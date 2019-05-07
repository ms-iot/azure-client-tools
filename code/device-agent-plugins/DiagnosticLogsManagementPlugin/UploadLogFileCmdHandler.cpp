// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/DMConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "UploadLogFileCmdHandler.h"
#include <was/storage_account.h>
#include <was/blob.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

constexpr char InterfaceVersion[] = "1.0.0";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace DiagnosticLogsManagementPlugin {

    UploadLogFile::UploadLogFile() :
        HandlerBase(UploadLogFileCmdHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, InterfaceVersion))
    {
    }

    void UploadLogFile::Start(
        const Json::Value& handlerConfig,
        bool& active)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        SetConfig(handlerConfig);

        Json::Value logFilesPath = handlerConfig[JsonTextLogFilesPath];
        Json::Value dataFilesPath = handlerConfig[JsonPluginsDataPath];
        _dataFolder = dataFilesPath.asString();

        if (!logFilesPath.isNull() && logFilesPath.isString())
        {
            wstring wideLogFileName = MultibyteToWide(logFilesPath.asString().c_str());
            wstring wideLogFileNamePrefix = MultibyteToWide(GetId().c_str());
            gLogger.SetLogFilePath(wideLogFileName.c_str(), wideLogFileNamePrefix.c_str());
            gLogger.EnableConsole(true);

            TRACELINE(LoggingLevel::Verbose, "Logging configured.");
        }

        active = true;
    }

    void UploadLogFile::OnConnectionStatusChanged(
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

    InvokeResult UploadLogFile::Invoke(
        const Json::Value& jsonParameters) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDirectMethod, JsonDirectMethodSuccessCode, JsonDirectMethodEmptyPayload);

        // Twin reported objects
        //no object is reported on success
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        Operation::RunOperation(GetId(), errorList,
            [&]()
        {
            // Process Meta Data
            _metaData->FromJsonParentObject(jsonParameters);
            string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

            //Compare interface version with the interface version sent by service
            if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
            {
                string folderName = Operation::GetStringJsonValue(jsonParameters, PayloadFolderName);
                string fileName = Operation::GetStringJsonValue(jsonParameters, PayloadFileName);
                string connectionString = Operation::GetStringJsonValue(jsonParameters, PayloadConnectionString);
                string containerName = Operation::GetStringJsonValue(jsonParameters, PayloadContainer);

                wstring fullFileName = MultibyteToWide((_dataFolder + "\\" + folderName + "\\" + fileName).c_str());

                TRACELINEP(LoggingLevel::Verbose, L"Uploading: ", fullFileName.c_str());

                // Retrieve storage account from connection string.
                auto storageAccount = azure::storage::cloud_storage_account::parse(MultibyteToWide(connectionString.c_str()));

                // Create the blob client.
                auto blobClient = storageAccount.create_cloud_blob_client();

                // Retrieve a reference to a previously created container.
                azure::storage::cloud_blob_container container = blobClient.get_container_reference(MultibyteToWide(containerName.c_str()));
                // Create the container if it doesn't already exist.
                container.create_if_not_exists();

                // Retrieve reference to a blob named fileName or creates one
                //if the filename is the same as an existing one, it will be overridden in the blob
                azure::storage::cloud_block_blob blockBlob = container.get_block_blob_reference(MultibyteToWide(fileName.c_str()));

                //get file from IoTDeviceAgent folder
                concurrency::streams::istream input_stream = concurrency::streams::file_stream<uint8_t>::open_istream(fullFileName).get();

                //uploads file to the blob
                blockBlob.upload_from_stream(input_stream);
                input_stream.close().wait();

                _metaData->SetDeviceInterfaceVersion(InterfaceVersion);
            }
            else
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_INTERFACE_VERSION, "Service solution is trying to talk with Interface Version that is not supported.");
            }

        });
        // Update device twin
        // This direct method doesn't update the twin.

        // Pack return payload
        if (errorList->Count() != 0)
        {
            invokeResult.code = JsonDirectMethodFailureCode;
            invokeResult.payload = errorList->ToJsonObject()[GetId()].toStyledString();
        }
        return invokeResult;
    }

}}}}