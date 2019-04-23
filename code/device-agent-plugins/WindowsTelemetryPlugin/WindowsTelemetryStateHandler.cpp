// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "WindowsTelemetryStateHandler.h"

// Windows Telemetry 0xA0008100 - 0xA00081FF
#define ERROR_DM_WINDOWS_TELEMETRY_INVALID_LEVEL 0xA0008100
#define ERROR_DM_WINDOWS_TELEMETRY_MISSING_LEVEL 0xA0008101

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace WindowsTelemetryPlugin {

    const wchar_t* RegWindowsTelemetrySubKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\DataCollection";
    const wchar_t* RegAllowTelemetry = L"AllowTelemetry";

    WindowsTelemetryStateHandler::WindowsTelemetryStateHandler() :
        HandlerBase(WindowsTelemetryStateHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
    {
    }

    unsigned long WindowsTelemetryStateHandler::StringToLevel(
        const std::string& levelString)
    {
        unsigned int level = 0;

        if (levelString == JsonWindowsTelemetryLevelSecurity)
        {
            level = 0;
        }
        else if (levelString == JsonWindowsTelemetryLevelBasic)
        {
            level = 1;
        }
        else if (levelString == JsonWindowsTelemetryLevelEnhanced)
        {
            level = 2;
        }
        else if (levelString == JsonWindowsTelemetryLevelFull)
        {
            level = 3;
        }
        else
        {
            throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "Failed to get telemetry level from provided Json");
        }

        return level;
    }

    string WindowsTelemetryStateHandler::LevelToString(unsigned long level)
    {
        string levelString;
        switch (level)
        {
        case 0:
            levelString = JsonWindowsTelemetryLevelSecurity;
            break;
        case 1:
            levelString = JsonWindowsTelemetryLevelBasic;
            break;
        case 2:
            levelString = JsonWindowsTelemetryLevelEnhanced;
            break;
        case 3:
            levelString = JsonWindowsTelemetryLevelFull;
            break;
        default:
            {
                basic_ostringstream<char> msg;
                msg << "Unknown level: " << level;
                throw DMException(DMSubsystem::DeviceAgentPlugin, ERROR_DM_WINDOWS_TELEMETRY_INVALID_LEVEL, msg.str().c_str());
            }
        }

        return levelString;
    }

    void WindowsTelemetryStateHandler::Start(
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

    void WindowsTelemetryStateHandler::OnConnectionStatusChanged(
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

    void WindowsTelemetryStateHandler::GetSubGroupLevel(
        Json::Value& reportedObject,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        Operation::RunOperation(JsonWindowsTelemetryLevel, errorList,
            [&]()
        {
            unsigned long level;
            if (ERROR_SUCCESS != Registry::TryReadRegistryValue(RegWindowsTelemetrySubKey, RegAllowTelemetry, level))
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, ERROR_DM_WINDOWS_TELEMETRY_MISSING_LEVEL, "Error: User setting for Windows telemetry level is not defined.");
            }

            reportedObject[JsonWindowsTelemetryLevel] = Json::Value(LevelToString(level));
        });
    }

    void WindowsTelemetryStateHandler::SetSubGroupLevel(
        const Json::Value& groupDesiredConfigJson,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        Operation::RunOperation(JsonWindowsTelemetryLevel, errorList,
            [&]()
        {
            string levelString = Operation::GetSinglePropertyOpStringParameter(groupDesiredConfigJson, JsonWindowsTelemetryLevel);
            Registry::WriteRegistryValue(RegWindowsTelemetrySubKey, RegAllowTelemetry, StringToLevel(levelString));

            // Is configured?
            _isConfigured = true;
        });
    }

    void WindowsTelemetryStateHandler::BuildReported(
        Json::Value& reportedObject,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        GetSubGroupLevel(reportedObject, errorList);
    }

    void WindowsTelemetryStateHandler::EmptyReported(
        Json::Value& reportedObject)
    {
        Json::Value nullValue;

        reportedObject[JsonWindowsTelemetryLevel] = nullValue;
    }

    InvokeResult WindowsTelemetryStateHandler::Invoke(
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
            SetSubGroupLevel(groupDesiredConfigJson, errorList);

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
