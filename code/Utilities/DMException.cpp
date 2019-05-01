#include "stdafx.h"
#include "DMException.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    // This needs to be in sync with the above DMSubsystem enum declared above 
    std::map<DMSubsystem, std::string> DMException::dmSubsystemsMap =
    {
        { DeviceAgent, "DeviceAgent" },
        { Windows, "Windows" },
        { RPC, "RPC" },
        { XmlLite, "XmlLite" },
        { Limpet, "Limpet" },
        { DPS, "DPS" },
        { IotHub, "IotHub" },
        { MDM, "MDM" },
        { MDMOperation, "MDMOperation" },
        { DeviceAgentPlugin, "DeviceAgentPlugin" },
        { W32TM, "W32TM" },
        { BCDEdit, "BCDEdit" },
        { Linux, "Linux" }
    };

    void LogDMException(
        const DMException& ex,
        const std::string& message,
        const std::string& param)
    {
        TRACELINEP(LoggingLevel::Error, message.c_str(), param.c_str());

        std::stringstream ss;
        ss << "Code: " << ex.Code() << " Message: " << ex.Message();
        TRACELINE(LoggingLevel::Error, ss.str().c_str());
    }

    void LogStdException(
        const std::exception& ex,
        const std::string& message,
        const std::string& param)
    {
        TRACELINEP(LoggingLevel::Error, message.c_str(), param.c_str());

        std::stringstream ss;
        ss << " Message: " << ex.what();
        TRACELINE(LoggingLevel::Error, ss.str().c_str());
    }

}}}}