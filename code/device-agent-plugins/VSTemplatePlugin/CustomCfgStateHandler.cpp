#include "stdafx.h"
#include "device-agent\common\DMCommon.h"
#include "$safeprojectname$StateHandler.h"

using namespace DMUtils;
using namespace DMCommon;
using namespace std;

constexpr char InterfaceVersion[] = "1.0.0";

$safeprojectname$StateHandler::$safeprojectname$StateHandler() :
    BaseHandler($safeprojectname$StateHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, InterfaceVersion))
{
}

void $safeprojectname$StateHandler::Start(
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

void $safeprojectname$StateHandler::OnConnectionStatusChanged(
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

InvokeResult $safeprojectname$StateHandler::Invoke(
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

        string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

        //Compare interface version with the interface version sent by service
        if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
        {
            // Apply new state

            // Report current state
            if (_metaData->GetReportingMode() == JsonReportingModeDefault)
            {
            }
            else
            {
            }
            _metaData->SetOutputInterfaceVersion(InterfaceVersion);
        }
        else
        {
            throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_INTERFACE_VERSION, "Service solution is trying to talk with Interface Version that is not supported.");
        }
    });

    // Update device twin
    FinalizeAndReport(reportedObject, errorList);

    return invokeResult;
}
