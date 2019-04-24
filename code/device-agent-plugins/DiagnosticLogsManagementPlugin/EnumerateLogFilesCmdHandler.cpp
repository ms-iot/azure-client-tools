// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/DMConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "EnumerateLogFilesCmdHandler.h"
#include <filesystem>

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft {
    namespace Azure {
        namespace DeviceManagement {
            namespace DiagnosticLogsManagementPlugin {

                EnumerateLogFiles::EnumerateLogFiles() :
                    HandlerBase(EnumerateLogFilesCmdHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
                {
                }

                void EnumerateLogFiles::Start(
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

                void EnumerateLogFiles::OnConnectionStatusChanged(
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

                InvokeResult EnumerateLogFiles::Invoke(
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
                        _metaData->FromJsonParentObject(jsonParameters);
                        string folderName = Operation::GetSinglePropertyOpStringParameter(jsonParameters, PayloadFolderName);

                        wstring path = MultibyteToWide((_dataFolder + "\\" + folderName).c_str());

                        vector<string> fileList = GetFileSystemObjectNames(path, file_type::regular);
                        Json::Value listArray(Json::ValueType::arrayValue);
                        for (string fileName : fileList) {
                            listArray.append(Json::Value(fileName));
                        }
                        reportedObject[PayloadList] = listArray;
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

            }
        }
    }
}
