// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <exception>
#include <sstream> 
#include "Logger.h"
#include "DMString.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class DMException : public std::exception
    {
    public:
        DMException(long code) :
            _code(code)
        {
            TRACELINEP(Utils::LoggingLevel::Error, "Exception code   : ", code);
        }

        DMException(long code, const std::string& message) :
            _code(code),
            _debugMessage(message)
        {
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
    };

}}}}
