// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <exception>
#include <sstream> 
#include <map>
#include "Logger.h"
#include "DMString.h"
#include "DMErrors.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    // Note: If you are adding a new field in here, also update dmSubsystemsMap in this DMException.cpp 
    enum DMSubsystem
    {
        DeviceAgent = 0,
        Windows,
        RPC,
        XmlLite,
        Limpet,
        DPS,
        IotHub,
        MDM,
        MDMOperation,
        DeviceAgentPlugin,
        W32TM,
        BCDEdit,
        Linux,
        IotStartup
    };

    class DMException : public std::exception
    {
    public:
        DMException(DMSubsystem subSystem, long code, const std::string& message) :
            _code(code),
            _debugMessage(message)
        {
            _subSystem = dmSubsystemsMap.at(subSystem);
            TRACELINEP(Utils::LoggingLevel::Error, "SubSystem              : ", _subSystem.c_str());
            TRACELINEP(Utils::LoggingLevel::Error, "Exception code         : ", _code);
            TRACELINEP(Utils::LoggingLevel::Error, "Exception debug message: ", _debugMessage.c_str());
        }

        DMException(const std::string subSystem, long code, const std::string& message) :
            _subSystem(subSystem),
            _code(code),
            _debugMessage(message)
        {
            TRACELINEP(Utils::LoggingLevel::Error, "Exception sub-system   : ", _subSystem.c_str());
            TRACELINEP(Utils::LoggingLevel::Error, "Exception code         : ", _code);
            TRACELINEP(Utils::LoggingLevel::Error, "Exception debug message: ", _debugMessage.c_str());
        }

        std::string SubSystem() const
        {
            return _subSystem;
        }

        long Code() const
        {
            return _code;
        }

        std::string Message() const
        {
            return _debugMessage;
        }

        virtual std::string DisplayMessage() const
        {
            std::basic_ostringstream<char> displayMessage;
            displayMessage << "Error: [" << _subSystem << "] Code: " << _code << ", Message: " << _debugMessage;
            return displayMessage.str();
        }

    protected:
        long _code;
        std::string _debugMessage;
        std::string _subSystem;

    private:
        static std::map<DMSubsystem, std::string> dmSubsystemsMap;
    };

    void LogDMException(const DMException& ex, const std::string& message, const std::string& param);

    void LogStdException(const std::exception& ex, const std::string& message, const std::string& param);
}}}}