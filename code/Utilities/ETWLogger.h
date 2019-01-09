// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    enum LoggingLevel
    {
        Verbose = 0,
        Information = 1,
        Warning = 2,
        Error = 3,
        Critical = 4
    };

    class ETWLogger
    {
    public:

        ETWLogger();
        ~ETWLogger();
        void Log(const std::wstring& msg, LoggingLevel level);
        void Log(const std::string& msg, LoggingLevel level);
    };

}}}}
