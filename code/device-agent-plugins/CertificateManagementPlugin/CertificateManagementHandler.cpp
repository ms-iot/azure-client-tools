// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CertificateManagementHandler.h"
#include "CertificateFile.h"
#include <was/storage_account.h>
#include <was/blob.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>
#include "PluginJsonConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"

#define CSPRootPath "./Device/Vendor/MSFT/RootCATrustedCertificates/Root"
#define CSPCAPath "./Device/Vendor/MSFT/RootCATrustedCertificates/CA"
#define CSPMyUserPath "./Vendor/MSFT/CertificateStore/My/User"
#define CSPMySystemPath "./Vendor/MSFT/CertificateStore/My/System"
#define CSPCASystemPath "./Vendor/MSFT/CertificateStore/CA/System"
#define CSPRootSystemPath "./Vendor/MSFT/CertificateStore/Root/System"
#define CSPTrustedPublisherPath "./Device/Vendor/MSFT/RootCATrustedCertificates/TrustedPublisher"
#define CSPTrustedPeoplePath "./Device/Vendor/MSFT/RootCATrustedCertificates/TrustedPeople"

using namespace DMUtils;
using namespace DMCommon;
using namespace std;

const char CertificateSeparator = '/';
const char FileConfigurationSeparator = '\\';
const string addEncodedCertificate = "/EncodedCertificate";

constexpr char InterfaceVersion[] = "1.0.0";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace CertificateManagementPlugin {

    CertificateManagementHandler::CertificateManagementHandler() :
        MdmHandlerBase(CertificateManagementHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, InterfaceVersion))
    {
    }

    string CertificateManagementHandler::DownloadCertificateFromBlob(
        const string& connectionString,
        const string& containerAndBlob,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        vector<string> fileConfigurationParts;
        SplitString(containerAndBlob, FileConfigurationSeparator, fileConfigurationParts);
        if (fileConfigurationParts.size() < (int)2)
        {
            throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "File Name format is incorrect. It should be <ContainerName>\\<BlobName>");
        }

        wstring containerName = MultibyteToWide(fileConfigurationParts[0].c_str());
        wstring blobName = MultibyteToWide(containerAndBlob.substr(containerName.size() + 1).c_str());

        auto storageAccount = azure::storage::cloud_storage_account::parse(MultibyteToWide(connectionString.c_str()));
        auto blobClient = storageAccount.create_cloud_blob_client();

        auto container = blobClient.get_container_reference(containerName);

        concurrency::streams::container_buffer<vector<uint8_t>> buffer;
        concurrency::streams::ostream output_stream(buffer);
        auto binary_blob = container.get_block_blob_reference(blobName);
        binary_blob.download_to_stream(output_stream);

        string fullFileName = (WideToMultibyte(GetTempFolder().c_str()) + fileConfigurationParts[fileConfigurationParts.size() - 1]);

        TRACELINEP(LoggingLevel::Verbose, "Certificate File downloaded at: ", fullFileName.c_str());

        ofstream outfile(fullFileName, ofstream::binary);
        vector<unsigned char>& data = buffer.collection();
        outfile.write(reinterpret_cast<char *>(&data[0]), buffer.size());
        outfile.close();

        return fullFileName;
    }

    void CertificateManagementHandler::UninstallCertificateHandler(
        const string& cspPath,
        const string& hash,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Operation::RunOperation(UninstallCertificate, errorList,
            [&]()
        {
            string path = cspPath + '/' + hash;

            TRACELINEP(LoggingLevel::Verbose, "Uninstall : ", path.c_str());
            // write
            _mdmProxy.RunDelete(path);

            // Set configured
            _isConfigured = true;
        });
    }

    bool CertificateManagementHandler::InstallCertificateHandler(
        const string& cspPath,
        const string& hash,
        const string& certificateInBase64,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        return Operation::RunOperation(InstallCertificate, errorList,
            [&]()
        {
            string path = cspPath + '/' + hash + addEncodedCertificate;

            TRACELINEP(LoggingLevel::Verbose, "Install : ", path.c_str());
            // write
            _mdmProxy.RunAddDataBase64(path, certificateInBase64);

            // Set configured
            _isConfigured = true;

        });
    }

    void CertificateManagementHandler::ModifyCertificatesHandler(
        const string& subGroupId,
        const string& cspPath,
        const Json::Value& desiredConfig,
        const OperationModelT<string>& connectionString,
        map<string, vector<string>>& installedCertificates,
        shared_ptr<ReportedErrorList> errorList)
    {
        vector<CertificateFile> certificatesToInstall;
        vector<string> certificatesToUninstall;

        Operation::RunOperation(subGroupId, errorList,
            [&]()
        {
            // Get existing certificate in CSP path
            string currentCertInfo = _mdmProxy.RunGetString(cspPath);

            vector<string> currentHashesVector;
            SplitString(currentCertInfo, CertificateSeparator, currentHashesVector);

            // Get desired certificate states for the specified CSP path (subGroupId)
            OperationModel certStoreJsonObject = Operation::TryGetJsonValue(desiredConfig, subGroupId);
            if (certStoreJsonObject.present)
            {
                for (Json::Value::const_iterator itr = certStoreJsonObject.value.begin(); itr != certStoreJsonObject.value.end(); itr++)
                {
                    string thumbprint = itr.key().asString();

                    // For each cert
                    Operation::RunOperation(thumbprint, errorList, [&]() {

                        // retrieve the desired state
                        string certDesiredState = Operation::GetStringJsonValue(*itr, JsonCertificateState);  // Required parameter.

                        // is currently installed?
                        bool isCurrentlyInstalled = false;
                        for (const string& currentHash : currentHashesVector)
                        {
                            if (_stricmp(currentHash.c_str(), thumbprint.c_str()) == 0)
                            {
                                isCurrentlyInstalled = true;
                                break;
                            }
                        }

                        if (!isCurrentlyInstalled)
                        {
                            if (certDesiredState == JsonStateInstalled)
                            {
                                TRACELINEP(LoggingLevel::Verbose, "-- Certificate to be installed was not found on the device : ", thumbprint.c_str());

                                // Make sure parameters are present.
                                if (!connectionString.present)
                                {
                                    throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "An Azure Storage connection string is missing. It is required to install certificates.");
                                }

                                OperationModelT<string> containerAndBlob = Operation::TryGetStringJsonValue(*itr, JsonCertificateFileName);
                                if (!containerAndBlob.present)
                                {
                                    throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "The container/blob string is missing. It is required to install certificates.");
                                }
                                if (containerAndBlob.value == "")
                                {
                                    throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "File Name is empty. It should be <ContainerName>\\<BlobName>");
                                }

                                // Download to a local temporary file
                                string fullFileName = DownloadCertificateFromBlob(connectionString.value, containerAndBlob.value, errorList);
                                certificatesToInstall.push_back(CertificateFile(fullFileName));
                            }
                            else if (certDesiredState == JsonStateUninstalled)
                            {
                                TRACELINEP(LoggingLevel::Verbose, "-- Certificate to be uninstalled was not found on the device: ", thumbprint.c_str());
                            }
                            else
                            {
                                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "Certificate state should be installed or uninstalled.");
                            }
                        }
                        else
                        {
                            if (certDesiredState == JsonStateUninstalled)
                            {
                                TRACELINEP(LoggingLevel::Verbose, "-- Will be uninstalling: ", thumbprint.c_str());
                                certificatesToUninstall.push_back(thumbprint);
                            }
                            else if (certDesiredState == JsonStateInstalled)
                            {
                                TRACELINEP(LoggingLevel::Verbose, "-- Already installed: ", thumbprint.c_str());
                                installedCertificates[subGroupId].push_back(thumbprint);
                            }
                            else
                            {
                                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "Certificate state should be installed or uninstalled.");
                            }
                        }
                    });
                }
            }
        });

        // Uninstall certificates
        for (const string& certificateToUninstall : certificatesToUninstall)
        {
            TRACELINEP(LoggingLevel::Verbose, "Uninstalling: ", certificateToUninstall.c_str());
            UninstallCertificateHandler(cspPath, certificateToUninstall, errorList);
        }

        // Install certificates
        for (const CertificateFile& certificateToInstall : certificatesToInstall)
        {
            TRACELINEP(LoggingLevel::Verbose, "Installing: ", certificateToInstall.GetFullFileName().c_str());
            string certificateInBase64 = FileToBase64(certificateToInstall.GetFullFileName());
            if (InstallCertificateHandler(cspPath, certificateToInstall.GetThumbPrint(), certificateInBase64, errorList))
            {
                installedCertificates[subGroupId].push_back(certificateToInstall.GetThumbPrint());
            }

            // Delete temp file when out of scope
            remove(certificateToInstall.GetFullFileName().c_str());
        }
    }

    Json::Value CertificateManagementHandler::CreateInstalledCertificateJsonList(
        std::vector<std::string>& installedCertificates)
    {
        Json::Value reportedObject(Json::objectValue);
        if(!installedCertificates.empty())
        { 
            // Populate the json object
            string emptyString = "";

            for (const string& cert : installedCertificates)
            {
                reportedObject[cert] = Json::Value(emptyString.c_str());
            }
        }
        return reportedObject;
    }

    void CertificateManagementHandler::Start(
        const Json::Value& handlerConfig,
        bool& active)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        SetConfig(handlerConfig);

        // Text file logging...
        Json::Value logFilesPath = handlerConfig[JsonTextLogFilesPath];
        if (!logFilesPath.isNull() && logFilesPath.isString())
        {
            wstring wideLogFileName = MultibyteToWide(logFilesPath.asString().c_str());
            wstring wideLogFileNamePrefix = MultibyteToWide(CertificateManagementHandlerId);
            gLogger.SetLogFilePath(wideLogFileName.c_str(), wideLogFileNamePrefix.c_str());
            gLogger.EnableConsole(true);

            TRACELINE(LoggingLevel::Verbose, "Logging configured.");
        }

        active = true;
    }

    void CertificateManagementHandler::OnConnectionStatusChanged(
        DMCommon::ConnectionStatus status)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        if (status == ConnectionStatus::eOffline)
        {
            TRACELINE(LoggingLevel::Verbose, "Connection Status: Offline.");
        }
        else
        {
            TRACELINE(LoggingLevel::Verbose, "Connection Status: Online.");
        }
    }

    void CertificateManagementHandler::BuildReported(
        Json::Value& reportedObject,
        std::map<std::string, std::vector<std::string>>& installedCertificates,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        reportedObject[JsonCACertificateInfo] = CreateInstalledCertificateJsonList(installedCertificates[JsonCACertificateInfo]);
        reportedObject[JsonRootCertificateInfo] = CreateInstalledCertificateJsonList( installedCertificates[JsonRootCertificateInfo]);
        reportedObject[JsonMyUserCertificateInfo] = CreateInstalledCertificateJsonList( installedCertificates[JsonMyUserCertificateInfo]);
        reportedObject[JsonMySystemCertificateInfo] = CreateInstalledCertificateJsonList( installedCertificates[JsonMyUserCertificateInfo]);
        reportedObject[JsonRootSystemCertificateInfo] = CreateInstalledCertificateJsonList( installedCertificates[JsonRootSystemCertificateInfo]);
        reportedObject[JsonCASystemCertificateInfo] = CreateInstalledCertificateJsonList( installedCertificates[JsonCASystemCertificateInfo]);
        reportedObject[JsonTrustedPublisherCertificateInfo] = CreateInstalledCertificateJsonList(installedCertificates[JsonTrustedPublisherCertificateInfo]);
        reportedObject[JsonTrustedPeopleCertificateInfo] = CreateInstalledCertificateJsonList( installedCertificates[JsonTrustedPeopleCertificateInfo]);
    }

    void CertificateManagementHandler::EmptyReported(
        Json::Value& reportedObject)
    {
        Json::Value nullValue;
        reportedObject[JsonCACertificateInfo] = nullValue;
        reportedObject[JsonRootCertificateInfo] = nullValue;
        reportedObject[JsonMyUserCertificateInfo] = nullValue;
        reportedObject[JsonMySystemCertificateInfo] = nullValue;
        reportedObject[JsonRootSystemCertificateInfo] = nullValue;
        reportedObject[JsonCASystemCertificateInfo] = nullValue;
        reportedObject[JsonTrustedPublisherCertificateInfo] = nullValue;
        reportedObject[JsonTrustedPeopleCertificateInfo] = nullValue;
    }

    InvokeResult CertificateManagementHandler::Invoke(
        const Json::Value& desiredConfig) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDesiredState);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        Operation::RunOperation(GetId(), errorList,
            [&]()
        {
            // Make sure this is not a transient state
            if (Operation::IsRefreshing(desiredConfig))
            {
                return;
            }

            // Processing Meta Data
            _metaData->FromJsonParentObject(desiredConfig);
            string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

            // Report refreshing
            ReportRefreshing();

            //Compare interface version with the interface version sent by service
            if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
            {
                // Apply new state
                OperationModelT<string> connectionString = Operation::TryGetStringJsonValue(desiredConfig, JsonConnectionString);
                map<string, vector<string>> installedCertificates;
                ModifyCertificatesHandler(JsonRootCertificateInfo, CSPRootPath, desiredConfig, connectionString, installedCertificates, errorList);
                ModifyCertificatesHandler(JsonCACertificateInfo, CSPCAPath, desiredConfig, connectionString, installedCertificates, errorList);
                ModifyCertificatesHandler(JsonMyUserCertificateInfo, CSPMyUserPath, desiredConfig, connectionString, installedCertificates, errorList);
                ModifyCertificatesHandler(JsonMySystemCertificateInfo, CSPMySystemPath, desiredConfig, connectionString, installedCertificates, errorList);
                ModifyCertificatesHandler(JsonRootSystemCertificateInfo, CSPRootSystemPath, desiredConfig, connectionString, installedCertificates, errorList);
                ModifyCertificatesHandler(JsonCASystemCertificateInfo, CSPCASystemPath, desiredConfig, connectionString, installedCertificates, errorList);
                ModifyCertificatesHandler(JsonTrustedPublisherCertificateInfo, CSPTrustedPublisherPath, desiredConfig, connectionString, installedCertificates, errorList);
                ModifyCertificatesHandler(JsonTrustedPeopleCertificateInfo, CSPTrustedPeoplePath, desiredConfig, connectionString, installedCertificates, errorList);

                // Report current state
                if (_metaData->GetReportingMode() == JsonReportingModeDefault)
                {
                    BuildReported(reportedObject, installedCertificates, errorList);
                }
                else
                {
                    EmptyReported(reportedObject);
                }
                _metaData->SetDeviceInterfaceVersion(InterfaceVersion);
            }
            else 
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_INTERFACE_VERSION, "Service solution is trying to talk with Interface Version that is not supported.");
            }
        });

        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }

}}}}