// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class Process
    {
    public:
        static void Launch(
            const std::wstring& commandString,
            unsigned long& returnCode,
            std::string& output);

        static bool IsProcessRunning(
            const std::wstring& processName);

        static std::wstring GetProcessExePath(
            DWORD processID);
    };

}}}}
