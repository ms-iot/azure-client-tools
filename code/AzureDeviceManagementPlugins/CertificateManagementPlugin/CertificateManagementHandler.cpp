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
#include "..\..\AzureDeviceManagementCommon\DMConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"

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


namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace CertificateManagementPlugin {

    CertificateManagementHandler::CertificateManagementHandler() :
        BaseHandler(CertificateManagementHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
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
        if (fileConfigurationParts.size() != (int)2)
        {
            throw DMException(ErrorInvalidJsonFormat, "File Name format is incorrect. It should be <ContainerName>\\<BlobName>");
        }

        wstring blobName = MultibyteToWide(fileConfigurationParts[1].c_str());
        wstring containerName = MultibyteToWide(fileConfigurationParts[0].c_str());

        auto storageAccount = azure::storage::cloud_storage_account::parse(MultibyteToWide(connectionString.c_str()));
        auto blobClient = storageAccount.create_cloud_blob_client();

        auto container = blobClient.get_container_reference(containerName);

        concurrency::streams::container_buffer<vector<uint8_t>> buffer;
        concurrency::streams::ostream output_stream(buffer);
        auto binary_blob = container.get_block_blob_reference(blobName);
        binary_blob.download_to_stream(output_stream);

        string fullFileName = (WideToMultibyte(GetTempFolder().c_str()) + fileConfigurationParts[1]);

        TRACELINEP(LoggingLevel::Verbose, "Certificate File downloaded at: ", fullFileName.c_str());

        ofstream outfile(fullFileName, ofstream::binary);
        vector<unsigned char>& data = buffer.collection();
        outfile.write(reinterpret_cast<char *>(&data[0]), buffer.size());
        outfile.close();
        
        return fullFileName;
    }

    string CertificateManagementHandler::GetInstalledCertificatesHandler(
        const string& cspPath,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        string currentCertInfo = "<error>";

        RunOperation(InstalledCertificateInfo, errorList,
            [&]()
        {
            // Merge...
            // n/a because this operation is a single-field operation.

            // Parse...
            currentCertInfo = _mdmProxy.RunGetString(cspPath);
        });
        return currentCertInfo;
    }

    void CertificateManagementHandler::UninstallCertificateHandler(
        const string& cspPath,
        const string& hash,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        RunOperation(UninstallCertificate, errorList,
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

    void CertificateManagementHandler::InstallCertificateHandler(
        const string& cspPath,
        const string& hash,
        const string& certificateInBase64,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        RunOperation(InstallCertificate, errorList,
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
        shared_ptr<ReportedErrorList> errorList)
    {
        vector<CertificateFile> certificatesToInstall;
        vector<string> certificatesToUninstall;

        RunOperation(subGroupId, errorList,
            [&]()
        {
            // Get existing certificate in CSP path
            string currentCertInfo = _mdmProxy.RunGetString(cspPath);

            vector<string> currentHashesVector;
            SplitString(currentCertInfo, CertificateSeparator, currentHashesVector);
            
            // Get desired certificate states for the specified CSP path (subGroupId)
            OperationModel certStoreJsonObject = TryGetOptionalSinglePropertyOpParameter(desiredConfig, subGroupId);
            if (certStoreJsonObject.present)
            {
                for (Json::Value::const_iterator itr = certStoreJsonObject.value.begin(); itr != certStoreJsonObject.value.end(); itr++)
                {
                    string thumbprint = itr.key().asString();

                    // For each cert
                    RunOperation(thumbprint, errorList, [&]() {

                        // retrieve the desired state
                        string certDesiredState = GetSinglePropertyOpStringParameter(*itr, JsonCertificateState);  // Required parameter.

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
                                    throw DMException(ErrorInvalidJsonFormat, "An Azure Storage connection string is missing. It is required to install certificates.");
                                }

                                OperationModelT<string> containerAndBlob = TryGetOptionalSinglePropertyOpStringParameter(*itr, JsonCertificateFileName);
                                if (!containerAndBlob.present)
                                {
                                    throw DMException(ErrorInvalidJsonFormat, "The container/blob string is missing. It is required to install certificates.");
                                }
                                if (containerAndBlob.value == "")
                                {
                                    throw DMException(ErrorInvalidJsonFormat, "File Name is empty. It should be <ContainerName>\\<BlobName>");
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
                                throw DMException(ErrorInvalidJsonFormat, "Certificate state should be installed or uninstalled.");
                            }
                        }
                        else
                        {
                            if (certDesiredState == JsonStateUninstalled)
                            {
                                TRACELINEP(LoggingLevel::Verbose, "-- Will be uninstalling: ", thumbprint.c_str());
                                certificatesToUninstall.push_back(thumbprint);
                            }
                            else if (certDesiredState == JsonStateInstalled) {
                                TRACELINEP(LoggingLevel::Verbose, "-- Already installed: ", thumbprint.c_str());
                            }
                            else
                            {
                                throw DMException(ErrorInvalidJsonFormat, "Certificate state should be installed or uninstalled.");
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
            TRACELINEP(LoggingLevel::Verbose, "Installing: ", certificateToInstall.FullFileName().c_str());
            string certificateInBase64 = FileToBase64(certificateToInstall.FullFileName());
            InstallCertificateHandler(cspPath, certificateToInstall.ThumbPrint(), certificateInBase64, errorList);

            // Delete temp file when out of scope
            remove(certificateToInstall.FullFileName().c_str());
        }
    }

    Json::Value CertificateManagementHandler::CreateCertificateJsonList(
        const string& hashesList)
    {
        // Parse the list
        vector<string> currentHashesVector;
        SplitString(hashesList, CertificateSeparator, currentHashesVector);

        // Populate the json object
        string emptyString = "";
        Json::Value reportedObject(Json::objectValue);
        for (const string& currentHash : currentHashesVector)
        {
            reportedObject[currentHash] = Json::Value(emptyString.c_str());
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
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        reportedObject[JsonCACertificateInfo] = CreateCertificateJsonList(GetInstalledCertificatesHandler(CSPCAPath, errorList));
        reportedObject[JsonRootCertificateInfo] = CreateCertificateJsonList(GetInstalledCertificatesHandler(CSPRootPath, errorList));
        reportedObject[JsonMyUserCertificateInfo] = CreateCertificateJsonList(GetInstalledCertificatesHandler(CSPMyUserPath, errorList));
        reportedObject[JsonMySystemCertificateInfo] = CreateCertificateJsonList(GetInstalledCertificatesHandler(CSPMySystemPath, errorList));
        reportedObject[JsonRootSystemCertificateInfo] = CreateCertificateJsonList(GetInstalledCertificatesHandler(CSPRootSystemPath, errorList));
        reportedObject[JsonCASystemCertificateInfo] = CreateCertificateJsonList(GetInstalledCertificatesHandler(CSPCASystemPath, errorList));
        reportedObject[JsonTrustedPublisherCertificateInfo] = CreateCertificateJsonList(GetInstalledCertificatesHandler(CSPTrustedPublisherPath, errorList));
        reportedObject[JsonTrustedPeopleCertificateInfo] = CreateCertificateJsonList(GetInstalledCertificatesHandler(CSPTrustedPeoplePath, errorList));
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

        RunOperation(GetId(), errorList,
            [&]()
        {
            // Make sure this is not a transient state
            if (IsRefreshing(desiredConfig))
            {
                return;
            }

            // Processing Meta Data
            _metaData->FromJsonParentObject(desiredConfig);

            // Signal refreshing
            SignalRefreshing();

            // Apply new state
            OperationModelT<string> connectionString = TryGetOptionalSinglePropertyOpStringParameter(desiredConfig, JsonConnectionString);

            ModifyCertificatesHandler(JsonRootCertificateInfo, CSPRootPath, desiredConfig, connectionString, errorList);
            ModifyCertificatesHandler(JsonCACertificateInfo, CSPCAPath, desiredConfig, connectionString, errorList);
            ModifyCertificatesHandler(JsonMyUserCertificateInfo, CSPMyUserPath, desiredConfig, connectionString, errorList);
            ModifyCertificatesHandler(JsonMySystemCertificateInfo, CSPMySystemPath, desiredConfig, connectionString, errorList);
            ModifyCertificatesHandler(JsonRootSystemCertificateInfo, CSPRootSystemPath, desiredConfig, connectionString, errorList);
            ModifyCertificatesHandler(JsonCASystemCertificateInfo, CSPCASystemPath, desiredConfig, connectionString, errorList);
            ModifyCertificatesHandler(JsonTrustedPublisherCertificateInfo, CSPTrustedPublisherPath, desiredConfig, connectionString, errorList);
            ModifyCertificatesHandler(JsonTrustedPeopleCertificateInfo, CSPTrustedPeoplePath, desiredConfig, connectionString, errorList);

            // Report current state
            if (_metaData->GetReportingMode() == JsonReportingModeAlways)
            {
                BuildReported(reportedObject, errorList);
            }
            else
            {
                EmptyReported(reportedObject);
            }
        });

        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }

}}}}
