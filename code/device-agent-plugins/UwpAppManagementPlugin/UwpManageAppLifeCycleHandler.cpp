// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "UwpManageAppLifeCycleHandler.h"
#include "UwpHelpers.h"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Management::Deployment;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace UwpAppManagementPlugin {

UwpManageAppLifeCycleHandler::UwpManageAppLifeCycleHandler() :
    MdmHandlerBase(UwpManageAppLifeCycleHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
{
}

void UwpManageAppLifeCycleHandler::Start(
    const Json::Value& config,
    bool& active)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    Json::Value logFilesPath = config[JsonTextLogFilesPath];
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

void UwpManageAppLifeCycleHandler::OnConnectionStatusChanged(
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

InvokeResult UwpManageAppLifeCycleHandler::Invoke(
    const Json::Value& jsonParameters) noexcept
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
    InvokeResult invokeResult(InvokeContext::eDirectMethod, JsonDirectMethodSuccessCode, JsonDirectMethodEmptyPayload);

    shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

    Operation::RunOperation(GetId(), errorList,
        [&]()
    {
        // Processing Meta Data
        _metaData->FromJsonParentObject(jsonParameters);

        bool start = true;

        string action = Operation::GetSinglePropertyOpStringParameter(jsonParameters, JsonAppAction);
        if (_stricmp(action.c_str(), JsonStartAction) == 0)
        {
            start = true;
        }
        else if (_stricmp(action.c_str(), JsonStopAction) == 0)
        {
            start = false;
        }
        else
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON, "Invalid action type");
        }

        string pkgFamilyName = Operation::GetSinglePropertyOpStringParameter(jsonParameters, JsonPkgFamilyName);

        UwpHelpers::StartStopApp(pkgFamilyName, start);

    });

    // Pack return payload
    if (errorList->Count() != 0)
    {
        invokeResult.code = JsonDirectMethodFailureCode;
        invokeResult.payload = errorList->ToJsonObject()[GetId()].toStyledString();
    }

    return invokeResult;
}
}}}}
