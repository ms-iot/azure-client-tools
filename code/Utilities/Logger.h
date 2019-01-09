// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <sstream>
#include <mutex>

#include "ETWLogger.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class Logger
    {
    public:
        Logger();

        void SetLogFilePath(const std::wstring& logFilePath, const std::wstring& prefix);
        void EnableConsole(bool enable);

        void Log(Utils::LoggingLevel level, bool newLine, const char*  msg);
        void Log(Utils::LoggingLevel level, bool newLine, const wchar_t*  msg);

        template<class T>
        void Log(Utils::LoggingLevel level, bool newLine, const wchar_t* msg, T param)
        {
            std::basic_ostringstream<wchar_t> message;
            message << msg << param;
            Log(level, newLine, message.str().c_str());
        }

        void Log(Utils::LoggingLevel level, bool newLine, const char* msg, const char* param);
        void Log(Utils::LoggingLevel level, bool newLine, const char* msg, int param);

    private:
        std::mutex _mutex;

        std::wstring _logFileName;
        bool _console;
    };

Logger __declspec(selectany) gLogger;

}}}}

#define TRACE(level, msg) gLogger.Log(level, false, msg)
#define TRACEP(level, format, param) gLogger.Log(level, false, format, param)

#define TRACELINE(level, msg) gLogger.Log(level, true, msg)
#define TRACELINEP(level, format, param) gLogger.Log(level, true, format, param)

