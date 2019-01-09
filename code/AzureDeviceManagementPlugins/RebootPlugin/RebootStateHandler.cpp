// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\DMConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "RebootStateHandler.h"

#define CSPPath "./Device/Vendor/MSFT/Reboot/Schedule/"

#define CSPSingle "Single"
#define CSPDaily "DailyRecurrent"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace RebootPlugin {

    RebootStateHandler::RebootStateHandler() :
        BaseHandler(RebootStateHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
    {
        // ToDo: This is the last re-start of DM Client - not necessarily the last reboot time.
        _lastRebootTime = Utils::WideToMultibyte(DateTime::GetCurrentDateTimeString().c_str());
    }

    void RebootStateHandler::SetSubGroup(
        const Json::Value& desiredConfig,
        const string& cspNodeId,
        const string& operationId,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        RunOperation(operationId, errorList,
            [&]()
        {
            OperationModelT<string> operationModel = TryGetOptionalSinglePropertyOpStringParameter(desiredConfig, operationId);

            if (operationModel.present)
            {
                string path = string(CSPPath) + cspNodeId;

                // write
                _mdmProxy.RunSet(path, operationModel.value);

                // Set configured
                _isConfigured = true;
            }
        });
    }

    void RebootStateHandler::GetSubGroup(
        const string& cspNodeId,
        const string& operationId,
        Json::Value& reportedObject,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        RunOperation(operationId, errorList,
            [&]()
        {
            string path = string(CSPPath) + cspNodeId;

            // Read back
            string readValue = _mdmProxy.RunGetString(path);
            reportedObject[operationId] = Json::Value(readValue);
        });
    }

    void RebootStateHandler::Start(
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

    void RebootStateHandler::OnConnectionStatusChanged(
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

    void RebootStateHandler::BuildReported(
        Json::Value& reportedObject,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        GetSubGroup(CSPSingle, JsonSingleRebootTime, reportedObject, errorList);
        GetSubGroup(CSPDaily, JsonDailyRebootTime, reportedObject, errorList);

        // The following does not need to be contained in a subgroup.
        reportedObject[JsonLastBootTime] = Json::Value(_lastRebootTime);
    }

    void RebootStateHandler::EmptyReported(
        Json::Value& reportedObject)
    {
        Json::Value nullValue;
        reportedObject[JsonSingleRebootTime] = nullValue;
        reportedObject[JsonDailyRebootTime] = nullValue;
        reportedObject[JsonLastBootTime] = nullValue;
    }

    InvokeResult RebootStateHandler::Invoke(
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

            // Processing Meta Data
            _metaData->FromJsonParentObject(groupDesiredConfigJson);

            // Apply new state
            SetSubGroup(groupDesiredConfigJson, CSPSingle, JsonSingleRebootTime, errorList);
            SetSubGroup(groupDesiredConfigJson, CSPDaily, JsonDailyRebootTime, errorList);

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

        // Update device twin
        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }

}}}}
