// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "FactoryResetCmdHandler.h"

#define ImmediateRebootCSPPath "./Device/Vendor/MSFT/Reboot/RebootNow"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

constexpr char InterfaceVersion[] = "1.0.0";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace FactoryResetPlugin {

    FactoryResetCmdHandler::FactoryResetCmdHandler() :
        MdmHandlerBase(FactoryResetCmdHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, InterfaceVersion)),
        _testModeEnabled(false)
    {
    }

    void FactoryResetCmdHandler::Start(
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

    void FactoryResetCmdHandler::OnConnectionStatusChanged(
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

    InvokeResult FactoryResetCmdHandler::Invoke(
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
            // Processing Meta Data
            _metaData->FromJsonParentObject(desiredConfig);
            string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

            //Compare interface version with the interface version sent by service
            if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
            { 
                // Get required parameters first...
                string partitionGuid = Operation::GetStringJsonValue(desiredConfig, JsonFactoryResetPartitionGuid);

                // Get optional parameters...
                OperationModelT<bool> clearTpmDataModel = Operation::TryGetBoolJsonValue(desiredConfig, JsonFactoryResetClearTpm);

                // Start processing...
                if (clearTpmDataModel.present && clearTpmDataModel.value)
                {
                    if (!_testModeEnabled)
                    {
                        TpmSupport::ClearTpm();
                    }
                }

                unsigned long returnCode = 0;
                string output;
                wstring command;
                command += DMUtils::GetSystemRootFolderW();
                command += L"\\bcdedit.exe  /set {bootmgr} bootsequence {";
                command += Utils::MultibyteToWide(partitionGuid.c_str());
                command += L"}";

                TRACELINE(LoggingLevel::Verbose, command.c_str());

                if (!_testModeEnabled)
                {
                    Process::Launch(command, returnCode, output);
                }

                if (returnCode != 0)
                {
                    throw DMException(DMSubsystem::BCDEdit, returnCode, "Error: ApplyUpdate.exe returned an error code.");
                }

                if (!_testModeEnabled)
                {
                    _mdmProxy.RunExec(ImmediateRebootCSPPath);
                }
                _metaData->SetDeviceInterfaceVersion(InterfaceVersion);
            }
            else
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_INTERFACE_VERSION, "Service solution is trying to talk with Interface Version that is not supported.");
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