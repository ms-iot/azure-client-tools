// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "TestCmdHandler.h"

#define CSPPath "./Device/Vendor/MSFT/Test/TestNow"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace TestPlugin {

    TestCmdHandler::TestCmdHandler() :
        MdmHandlerBase(TestCmdHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
    {
    }

    void TestCmdHandler::Start(
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

    void TestCmdHandler::OnConnectionStatusChanged(
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

    InvokeResult TestCmdHandler::Invoke(
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
            string action = jsonParameters[TestCmdAction].asString();
            string type = jsonParameters[TestCmdReturnType].asString();

            if (action == TestCmdActionSucceed)
            {
                if (type == TestCmdReturnTypeObject)
                {
                    Json::Value returnJson(Json::objectValue);
                    returnJson[TestCmdReturnInt] = 5;
                    returnJson[TestCmdReturnString] = "abc";
                    invokeResult.payload = returnJson.toStyledString();
                }
                else if (type == TestCmdReturnTypeInteger)
                {
                    Json::Value returnJson(42);
                    invokeResult.payload = returnJson.toStyledString();
                }
                invokeResult.code = JsonDirectMethodSuccessCode;
            }
            else if (action == TestCmdActionFail)
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, PLUGIN_ERROR_CMD_FAILED, "Command failed as expected.");
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
