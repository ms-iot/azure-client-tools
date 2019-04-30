// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/DMConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "EnumerateLogFoldersCmdHandler.h"
#include <filesystem>

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

constexpr char InterfaceVersion[] = "1.0.0";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace DiagnosticLogsManagementPlugin {

    EnumerateLogFolders::EnumerateLogFolders() :
        HandlerBase(EnumerateLogFoldersCmdHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, InterfaceVersion))
    {
    }

    void EnumerateLogFolders::Start(
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

    void EnumerateLogFolders::OnConnectionStatusChanged(
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

    InvokeResult EnumerateLogFolders::Invoke(
        const Json::Value& jsonParameters) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDirectMethod, JsonDirectMethodSuccessCode, JsonDirectMethodEmptyPayload);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        Operation::RunOperation(GetId(), errorList,
            [&]()
        {
            // Process Meta Data
            //this direct method has no incoming json parameters
            _metaData->FromJsonParentObject(jsonParameters);
            string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

            //Compare interface version with the interface version sent by service
            if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
            {
                vector<string> folderList;
                wstring path = MultibyteToWide((_dataFolder + "\\").c_str());
                folderList = GetFileSystemObjectNames(path, file_type::directory);
                Json::Value listArray(Json::ValueType::arrayValue);
                for (string folderName : folderList) {
                    listArray.append(Json::Value(folderName));
                }
                reportedObject[PayloadList] = listArray;
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
        if (errorList->Count() == 0)
        {
            invokeResult.payload = reportedObject.toStyledString();
        }
        else
        {
            invokeResult.code = JsonDirectMethodFailureCode;
            invokeResult.payload = errorList->ToJsonObject()[GetId()].toStyledString();
        }
        return invokeResult;
    }

}}}}