// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\DMConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "WindowsUpdateHandler.h"

#define CSPWritePath "./Device/Vendor/MSFT/Policy/Config/Update/"
#define CSPReadPath "./Device/Vendor/MSFT/Policy/Result/Update/"

#define CSPActiveHoursStart "ActiveHoursStart"
#define CSPActiveHoursEnd "ActiveHoursEnd"
#define CSPAllowAutoUpdate "AllowAutoUpdate"
#define CSPAllowUpdateService "AllowUpdateService"
#define CSPBranchReadinessLevel "BranchReadinessLevel"
#define CSPDeferFeatureUpdatesPeriodInDays "DeferFeatureUpdatesPeriodInDays"
#define CSPDeferQualityUpdatesPeriodInDays "DeferQualityUpdatesPeriodInDays"
#define CSPPauseFeatureUpdates "PauseFeatureUpdates"
#define CSPPauseQualityUpdates "PauseQualityUpdates"
#define CSPScheduledInstallDay "ScheduledInstallDay"
#define CSPScheduledInstallTime "ScheduledInstallTime"

#define JsonActiveHoursStart "activeHoursStart"
#define JsonActiveHoursEnd "activeHoursEnd"
#define JsonAllowAutoUpdate "allowAutoUpdate"
#define JsonAllowUpdateService "allowUpdateService"
#define JsonBranchReadinessLevel "branchReadinessLevel"
#define JsonDeferFeatureUpdatesPeriodInDays "deferFeatureUpdatesPeriodInDays"
#define JsonDeferQualityUpdatesPeriodInDays "deferQualityUpdatesPeriodInDays"
#define JsonPauseFeatureUpdates "pauseFeatureUpdates"
#define JsonPauseQualityUpdates "pauseQualityUpdates"
#define JsonScheduledInstallDay "scheduledInstallDay"
#define JsonScheduledInstallTime "scheduledInstallTime"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace WindowsUpdatePlugin {

    WindowsUpdateHandler::WindowsUpdateHandler() :
        BaseHandler(WindowsUpdateHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
    {
    }

    void WindowsUpdateHandler::Start(
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

    void WindowsUpdateHandler::OnConnectionStatusChanged(
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

    void WindowsUpdateHandler::SetSubGroup(
        const Json::Value& groupDesiredConfigJson,
        const std::string& cspNodeId,
        const std::string& operationId,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        RunOperation(operationId, errorList,
            [&]()
        {
            OperationModelT<int> operationDataModel = TryGetOptionalSinglePropertyOpIntParameter(groupDesiredConfigJson, operationId);
            if (operationDataModel.present)
            {
                string writePath = string(CSPWritePath) + cspNodeId;
                _mdmProxy.RunSet(writePath, operationDataModel.value);

                // Is configured?
                _isConfigured = true;
            }
        });
    }

    void WindowsUpdateHandler::GetSubGroup(
        const std::string& cspNodeId,
        const std::string& operationId,
        Json::Value& reportedObject,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        RunOperation(operationId, errorList,
            [&]()
        {
            string readPath = string(CSPReadPath) + cspNodeId;

            int readValue;
            _mdmProxy.TryGetNumber<int>(readPath, readValue);
            reportedObject[operationId] = Json::Value(readValue);
        });
    }

    void WindowsUpdateHandler::BuildReported(
        Json::Value& reportedObject,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        GetSubGroup(CSPActiveHoursStart, JsonActiveHoursStart, reportedObject, errorList);
        GetSubGroup(CSPActiveHoursEnd, JsonActiveHoursEnd, reportedObject, errorList);
        GetSubGroup(CSPAllowAutoUpdate, JsonAllowAutoUpdate, reportedObject, errorList);
        GetSubGroup(CSPAllowUpdateService, JsonAllowUpdateService, reportedObject, errorList);
        GetSubGroup(CSPBranchReadinessLevel, JsonBranchReadinessLevel, reportedObject, errorList);
        GetSubGroup(CSPDeferFeatureUpdatesPeriodInDays, JsonDeferFeatureUpdatesPeriodInDays, reportedObject, errorList);
        GetSubGroup(CSPDeferQualityUpdatesPeriodInDays, JsonDeferQualityUpdatesPeriodInDays, reportedObject, errorList);
        GetSubGroup(CSPPauseFeatureUpdates, JsonPauseFeatureUpdates, reportedObject, errorList);
        GetSubGroup(CSPPauseQualityUpdates, JsonPauseQualityUpdates, reportedObject, errorList);
        GetSubGroup(CSPScheduledInstallDay, JsonScheduledInstallDay, reportedObject, errorList);
        GetSubGroup(CSPScheduledInstallTime, JsonScheduledInstallTime, reportedObject, errorList);
    }

    void WindowsUpdateHandler::EmptyReported(
        Json::Value& reportedObject)
    {
        Json::Value nullValue;
        reportedObject[JsonActiveHoursStart] = nullValue;
        reportedObject[JsonActiveHoursEnd] = nullValue;
        reportedObject[JsonAllowAutoUpdate] = nullValue;
        reportedObject[JsonAllowUpdateService] = nullValue;
        reportedObject[JsonBranchReadinessLevel] = nullValue;
        reportedObject[JsonDeferFeatureUpdatesPeriodInDays] = nullValue;
        reportedObject[JsonDeferQualityUpdatesPeriodInDays] = nullValue;
        reportedObject[JsonPauseFeatureUpdates] = nullValue;
        reportedObject[JsonPauseQualityUpdates] = nullValue;
        reportedObject[JsonScheduledInstallDay] = nullValue;
        reportedObject[JsonScheduledInstallTime] = nullValue;
    }

    InvokeResult WindowsUpdateHandler::Invoke(
        const Json::Value& groupDesiredConfigJson) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDesiredState);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        RunOperation(GetId(), errorList,
            [&]()
        {
            // Make sure this is not a transient state
            if (IsRefreshing(groupDesiredConfigJson))
            {
                return;
            }

            // Process Meta Data
            _metaData->FromJsonParentObject(groupDesiredConfigJson);

            // Apply new state
            SetSubGroup(groupDesiredConfigJson, CSPActiveHoursStart, JsonActiveHoursStart, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPActiveHoursEnd, JsonActiveHoursEnd, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPAllowAutoUpdate, JsonAllowAutoUpdate, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPAllowUpdateService, JsonAllowUpdateService, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPBranchReadinessLevel, JsonBranchReadinessLevel, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPDeferFeatureUpdatesPeriodInDays, JsonDeferFeatureUpdatesPeriodInDays, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPDeferQualityUpdatesPeriodInDays, JsonDeferQualityUpdatesPeriodInDays, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPPauseFeatureUpdates, JsonPauseFeatureUpdates, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPPauseQualityUpdates, JsonPauseQualityUpdates, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPScheduledInstallDay, JsonScheduledInstallDay, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPScheduledInstallTime, JsonScheduledInstallTime, errorList);

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
