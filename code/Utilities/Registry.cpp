// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <vector>
#include "DMException.h"
#include "Registry.h"

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    void Registry::WriteRegistryValue(const wstring& subKey, const wstring& propName, const wstring& propValue)
    {
        HKEY hKey = NULL;
        LSTATUS status = RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,
            subKey.c_str(),
            0,      // reserved
            NULL,   // user-defined class type of this key.
            0,      // default; non-volatile
            KEY_ALL_ACCESS,
            NULL,   // inherit security descriptor from parent.
            &hKey,
            NULL    // disposition [optional, out]
        );
        if (status != ERROR_SUCCESS)
        {
            throw DMException(status);
        }

        status = RegSetValueEx(hKey, propName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(propValue.c_str()), (static_cast<unsigned int>(propValue.size()) + 1) * sizeof(propValue[0]));
        if (status != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            throw DMException(status);
        }

        RegCloseKey(hKey);
    }

    void Registry::WriteRegistryValue(const wstring& subKey, const wstring& propName, unsigned long propValue)
    {
        HKEY hKey = NULL;
        LSTATUS status = RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,
            subKey.c_str(),
            0,      // reserved
            NULL,   // user-defined class type of this key.
            0,      // default; non-volatile
            KEY_ALL_ACCESS,
            NULL,   // inherit security descriptor from parent.
            &hKey,
            NULL    // disposition [optional, out]
        );
        if (status != ERROR_SUCCESS)
        {
            throw DMException(status);
        }

        status = RegSetValueEx(hKey, propName.c_str(), 0, REG_DWORD, reinterpret_cast<BYTE*>(&propValue), sizeof(propValue));
        if (status != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            throw DMException(status);
        }

        RegCloseKey(hKey);
    }

    LSTATUS Registry::TryReadRegistryValue(const wstring& subKey, const wstring& propName, wstring& propValue)
    {
        DWORD dataSize = 0;
        LSTATUS status = RegGetValue(HKEY_LOCAL_MACHINE, subKey.c_str(), propName.c_str(), RRF_RT_REG_SZ, NULL, NULL, &dataSize);
        if (status != ERROR_SUCCESS)
        {
            return status;
        }

        vector<char> data(dataSize);
        status = RegGetValue(HKEY_LOCAL_MACHINE, subKey.c_str(), propName.c_str(), RRF_RT_REG_SZ, NULL, data.data(), &dataSize);
        if (status != ERROR_SUCCESS)
        {
            return status;
        }

        propValue = reinterpret_cast<const wchar_t*>(data.data());

        return ERROR_SUCCESS;
    }

    LSTATUS Registry::TryReadRegistryValue(const wstring& subKey, const wstring& propName, unsigned long& propValue)
    {
        DWORD data = 0;
        DWORD dataSize = sizeof(data);
        LSTATUS status = RegGetValue(HKEY_LOCAL_MACHINE, subKey.c_str(), propName.c_str(), RRF_RT_REG_DWORD, NULL, &data, &dataSize);
        if (status != ERROR_SUCCESS)
        {
            return status;
        }

        propValue = data;
        return ERROR_SUCCESS;
    }

    wstring Registry::ReadRegistryValue(const wstring& subKey, const wstring& propName)
    {
        wstring propValue;
        LSTATUS status = TryReadRegistryValue(subKey, propName, propValue);
        if (status != ERROR_SUCCESS)
        {
            TRACELINEP(LoggingLevel::Error, L"Error: Could not read registry value: ", (subKey + L"\\" + propName).c_str());
            throw DMException(status);
        }
        return propValue;
    }

    wstring Registry::ReadRegistryValue(const wstring& subKey, const wstring& propName, const wstring& propDefaultValue)
    {
        wstring propValue;
        if (ERROR_SUCCESS != TryReadRegistryValue(subKey, propName, propValue))
        {
            propValue = propDefaultValue;
        }
        return propValue;
    }

    bool Registry::ReadRegistryBoolValue(const wstring& subKey, const wstring& propName, bool propDefaultValue)
    {
        unsigned long propValue;
        if (ERROR_SUCCESS != TryReadRegistryValue(subKey, propName, propValue))
        {
            propValue = propDefaultValue ? 1 : 0;
        }
        return propValue != 0;
    }

    int Registry::ReadRegistryInt32Value(const wstring& subKey, const wstring& propName, int propDefaultValue)
    {
        unsigned long propValue;
        if (ERROR_SUCCESS != TryReadRegistryValue(subKey, propName, propValue))
        {
            propValue = propDefaultValue;
        }
        return propValue;
    }

}}}}
