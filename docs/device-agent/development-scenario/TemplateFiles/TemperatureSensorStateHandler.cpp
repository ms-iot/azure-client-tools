#include "stdafx.h"
#include "AzureDeviceManagementCommon\DMCommon.h"
#include "TemperatureSensorStateHandler.h"

using namespace DMUtils;
using namespace DMCommon;
using namespace std;

TemperatureSensorStateHandler::TemperatureSensorStateHandler() :
    BaseHandler(TemperatureSensorStateHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
{
}

void TemperatureSensorStateHandler::Start(
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

void TemperatureSensorStateHandler::OnConnectionStatusChanged(
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

InvokeResult TemperatureSensorStateHandler::Invoke(
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

        // Report current state
        if (_metaData->GetReportingMode() == JsonReportingModeAlways)
        {
        }
        else
        {
        }
    });

    // Update device twin
    FinalizeAndReport(reportedObject, errorList);

    return invokeResult;
}


