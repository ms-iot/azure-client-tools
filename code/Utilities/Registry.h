// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <string>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class Registry
    {
    public:
        static void WriteRegistryValue(const std::wstring& subKey, const std::wstring& propName, const std::wstring& propValue);
        static void WriteRegistryValue(const std::wstring& subKey, const std::wstring& propName, unsigned long propValue);
        static LSTATUS TryReadRegistryValue(const std::wstring& subKey, const std::wstring& propName, std::wstring& propValue);
        static LSTATUS TryReadRegistryValue(const std::wstring& subKey, const std::wstring& propName, unsigned long& propValue);
        static std::wstring ReadRegistryValue(const std::wstring& subKey, const std::wstring& propName);
        static std::wstring ReadRegistryValue(const std::wstring& subKey, const std::wstring& propName, const std::wstring& propDefaultValue);
        static bool ReadRegistryBoolValue(const std::wstring& subKey, const std::wstring& propName, bool propDefaultValue);
        static int ReadRegistryInt32Value(const std::wstring& subKey, const std::wstring& propName, int propDefaultValue);
    };

}}}}
