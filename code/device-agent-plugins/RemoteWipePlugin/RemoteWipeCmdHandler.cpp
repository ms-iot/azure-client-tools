// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "RemoteWipeCmdHandler.h"

#define RemoteWipeCSPPath "./Vendor/MSFT/RemoteWipe/doWipe"
#define ImmediateRebootCSPPath "./Device/Vendor/MSFT/Reboot/RebootNow"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace RemoteWipePlugin {

    RemoteWipeCmdHandler::RemoteWipeCmdHandler() :
        MdmHandlerBase(RemoteWipeCmdHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1)),
        _testModeEnabled(false)
    {
    }

    void RemoteWipeCmdHandler::Start(
        const Json::Value& handlerConfig,
        bool& active)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        SetConfig(handlerConfig);

        Json::Value logFilesPath = handlerConfig[JsonTextLogFilesPath];
        if (!logFilesPath.isNull() && logFilesPath.isString())
        {
            wstring wideLogFileName = MultibyteToWide(logFilesPath.asString().c_str());
            wstring wideLogFileNamePrefix = MultibyteToWide(GetId().c_str());
            gLogger.SetLogFilePath(wideLogFileName.c_str(), wideLogFileNamePrefix.c_str());
            gLogger.EnableConsole(true);

            TRACELINE(LoggingLevel::Verbose, "Logging configured.");
        }

        Json::Value handlerTestMode = handlerConfig[JsonHandlerTestMode];
        if (!handlerTestMode.isNull() && handlerTestMode.isBool())
        {
            _testModeEnabled = handlerTestMode.asBool();
            if (_testModeEnabled)
            {
                TRACELINE(LoggingLevel::Verbose, "Test mode is enabled.");
            }
            else
            {
                TRACELINE(LoggingLevel::Verbose, "Test mode is disabled.");
            }
        }

        active = true;
    }

    void RemoteWipeCmdHandler::OnConnectionStatusChanged(
        ConnectionStatus status)
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

    InvokeResult RemoteWipeCmdHandler::Invoke(
        const Json::Value& desiredConfig) noexcept
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
            OperationModelT<bool> operationDataModel = Operation::TryGetOptionalSinglePropertyOpBoolParameter(desiredConfig, JsonRemoteWipeClearTpm);
            if (operationDataModel.present && operationDataModel.value)
            {
                if (!_testModeEnabled)
                {
                    TpmSupport::ClearTpm();
                }
            }

            if (!_testModeEnabled)
            {
                _mdmProxy.RunExec(RemoteWipeCSPPath);
                _mdmProxy.RunExec(ImmediateRebootCSPPath);
            }
        });

        // Update device twin
        FinalizeAndReport(reportedObject, errorList);

        // Pack return payload
        if (errorList->Count() != 0)
        {
            invokeResult.code = JsonDirectMethodFailureCode;
            invokeResult.payload = errorList->ToJsonObject()[GetId()].toStyledString();
        }
        return invokeResult;
    }

}}}}
