#include "stdafx.h"
#include "device-agent\common\DMCommon.h"
#include "$safeprojectname$Cmd1Handler.h"

using namespace DMUtils;
using namespace DMCommon;
using namespace std;

$safeprojectname$Cmd1Handler::$safeprojectname$Cmd1Handler() :
    HandlerBase($safeprojectname$Cmd1HandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
{
}

void $safeprojectname$Cmd1Handler::Start(
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

void $safeprojectname$Cmd1Handler::OnConnectionStatusChanged(
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

InvokeResult $safeprojectname$Cmd1Handler::Invoke(
    const Json::Value& groupDesiredConfigJson) noexcept
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
        // Make sure this is not a transient state
        if (Operation::IsRefreshing(groupDesiredConfigJson))
        {
            return;
        }

        // Processing Meta Data
        _metaData->FromJsonParentObject(groupDesiredConfigJson);

        // Apply new state

        // Report current state
        if (_metaData->GetReportingMode() == JsonReportingModeDetailed)
        {
        }
        else
        {
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

