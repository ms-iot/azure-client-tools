// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <string>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class ServiceManager
    {
    public:
        static DWORD GetStatus(const std::wstring& serviceName);
        static DWORD GetStartType(const std::wstring& serviceName);

        static void Start(const std::wstring& serviceName);
        static void Stop(const std::wstring& serviceName);
        static void SetStartType(const std::wstring& serviceName, DWORD startType);

        static void WaitStatus(const std::wstring& serviceName, DWORD status, unsigned int maxWaitInSeconds);
    private:
        static void StartStop(const std::wstring& serviceName, bool start);
    };

}}}}
