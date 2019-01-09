// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\DMConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "FactoryResetCmdHandler.h"

#define FactoryResetCSPPath "./Vendor/MSFT/FactoryReset/doWipe"
#define ImmediateRebootCSPPath "./Device/Vendor/MSFT/Reboot/RebootNow"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace FactoryResetPlugin {

    FactoryResetCmdHandler::FactoryResetCmdHandler() :
        BaseHandler(FactoryResetCmdHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
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

        (void)desiredConfig;

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDirectMethod, JsonDirectMethodSuccessCode, JsonDirectMethodEmptyPayload);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        RunOperation(GetId(), errorList,
            [&]()
        {
            // Get required parameters first...
            string partitionGuid = GetSinglePropertyOpStringParameter(desiredConfig, JsonFactoryResetPartitionGuid);

            // Get optional parameters...
            OperationModelT<bool> clearTpmDataModel = TryGetOptionalSinglePropertyOpBoolParameter(desiredConfig, JsonFactoryResetClearTpm);

            // Start processing...
            if (clearTpmDataModel.present && clearTpmDataModel.value)
            {
                TpmSupport::ClearTpm();
            }

            unsigned long returnCode = 0;
            string output;
            wstring command;
            command += DMUtils::GetSystemRootFolderW();
            command += L"\\bcdedit.exe  /set {bootmgr} bootsequence {";
            command += Utils::MultibyteToWide(partitionGuid.c_str());
            command += L"}";

            TRACELINE(LoggingLevel::Verbose, command.c_str());

            Process::Launch(command, returnCode, output);
            if (returnCode != 0)
            {
                throw DMException(returnCode, "Error: ApplyUpdate.exe returned an error code.");
            }

            _mdmProxy.RunExec(ImmediateRebootCSPPath);
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
