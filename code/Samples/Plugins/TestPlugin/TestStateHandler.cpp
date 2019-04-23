// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "TestStateHandler.h"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace TestPlugin {

    TestStateHandler::TestStateHandler() :
        HandlerBase(TestStateHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
    {
    }

    void TestStateHandler::Start(
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

    void TestStateHandler::OnConnectionStatusChanged(
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

    void TestStateHandler::BuildReported(
        Json::Value& reportedObject,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        Operation::RunOperation(TestStateAction, errorList,
            [&]()
        {
            reportedObject[TestStateAction] = Json::Value(_action);
        });
    }

    void TestStateHandler::EmptyReported(
        Json::Value& reportedObject)
    {
        Json::Value nullValue;
        reportedObject[TestStateAction] = nullValue;
    }

    InvokeResult TestStateHandler::Invoke(
        const Json::Value& groupDesiredConfigJson) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDesiredState);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        Operation::RunOperation(GetId(), errorList,
            [&]()
        {
            // Make sure this is not a transient state
            if (Operation::IsRefreshing(groupDesiredConfigJson))
            {
                return;
            }

            // Processing Meta Data
            _metaData->FromJsonParentObject(groupDesiredConfigJson);

            // Apply new state
            // SetSubGroup(groupDesiredConfigJson, CSPSingle, JsonSingleTestTime, errorList);

            Operation::RunOperation(TestStateAction, errorList,
                [&]()
            {
                OperationModelT<string> operationModel = Operation::TryGetOptionalSinglePropertyOpStringParameter(groupDesiredConfigJson, TestStateAction);

                if (operationModel.present)
                {
                    _action = operationModel.value;
                    if (_action == "succeed")
                    {
                    }
                    else if (_action == "fail")
                    {
                        throw DMException(DMSubsystem::DeviceAgentPlugin, PLUGIN_ERROR_STATE_FAILED, "State setting failed as expected.");
                    }
                }
            });


            // Report current state
            if (_metaData->GetReportingMode() == JsonReportingModeDetailed)
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
