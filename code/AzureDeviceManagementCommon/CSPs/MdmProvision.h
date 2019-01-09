// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "SyncMLHelpers.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

class MdmProvision
{
public:
    // With sid
    static void RunSyncML(const std::wstring& sid, const std::wstring& inputSyncML, std::wstring& outputSyncML);

    static void RunAdd(const std::wstring& sid, const std::wstring& path, const std::wstring& value);
    static void RunAddData(const std::wstring& sid, const std::wstring& path, const std::wstring& value, const std::wstring& type = L"chr");
    static void RunAddTyped(const std::wstring& sid, const std::wstring& path, const std::wstring& type);
    static void RunAddDataBase64(const std::wstring& sid, const std::wstring& path, const std::wstring& value);

    static void RunDelete(const std::wstring& sid, const std::wstring& path);

    static void RunGetStructData(const std::wstring& path, ELEMENT_HANDLER handler);
    static std::wstring RunGetString(const std::wstring& sid, const std::wstring& path);
    static std::wstring RunGetBase64(const std::wstring& sid, const std::wstring& path);
    static unsigned int RunGetUInt(const std::wstring& sid, const std::wstring& path);
    static bool RunGetBool(const std::wstring& sid, const std::wstring& path);

    static void RunSet(const std::wstring& sid, const std::wstring& path, const std::wstring& value);
    static void RunSet(const std::wstring& sid, const std::wstring& path, int value);
    static void RunSet(const std::wstring& sid, const std::wstring& path, bool value);
    static void RunSetBase64(const std::wstring& sid, const std::wstring& path, const std::wstring& value);

    static void RunExec(const std::wstring& sid, const std::wstring& path);
    static void RunExecWithParameters(const std::wstring& sid, const std::wstring& path, const std::wstring& params);

    // Without sid
    static void RunAdd(const std::wstring& path, const std::wstring& value);
    static void RunAddData(const std::wstring& path, const std::wstring& value);
    static void RunAddTyped(const std::wstring& path, const std::wstring& type);
    static void RunAddDataBase64(const std::wstring& path, const std::wstring& value);
    static void RunAddData(const std::wstring& path, int value);
    static void RunAddData(const std::wstring& path, bool value);

    static void RunDelete(const std::wstring& path);

    static std::wstring RunGetString(const std::wstring& path);
    static bool TryGetString(const std::wstring& path, std::wstring& value);
    static std::wstring RunGetBase64(const std::wstring& path);
    static unsigned int RunGetUInt(const std::wstring& path);

    template<class T>
    static bool TryGetNumber(const std::wstring& path, std::wstring& value)
    {
        bool success = true;
        try
        {
            T number = static_cast<T>(RunGetUInt(path));
            value = Utils::MultibyteToWide(std::to_string(number).c_str());
        }
        catch (typename DMUtils::DMException&)
        {
            // ToDo: should we log these in debug?
            success = false;
        }
        return success;
    }

    template<class T>
    static bool TryGetNumber(const std::wstring& path, T& value)
    {
        bool success = true;
        try
        {
            value = static_cast<T>(RunGetUInt(path));
        }
        catch (typename DMUtils::DMException&)
        {
            // ToDo: should we log these in debug?
            success = false;
        }
        return success;
    }

    static bool RunGetBool(const std::wstring& path);
    static bool TryGetBool(const std::wstring& path, bool& value);

    static void RunSet(const std::wstring& path, const std::wstring& value);
    static void RunSet(const std::wstring& path, int value);
    static void RunSet(const std::wstring& path, bool value);
    static void RunSetBase64(const std::wstring& path, const std::wstring& value);

    static void RunExec(const std::wstring& path);
    static void RunExecWithParameters(const std::wstring& path, const std::wstring& params);

    // Helpers
    static std::wstring GetErrorMessage(const std::wstring& returnCodeString);
    static void ReportError(const std::wstring& syncMLRequest, const std::wstring& syncMLResponse, int errorCode);
    static void ReportError(const std::wstring& syncMLRequest, const std::wstring& syncMLResponse);
};

}}}}
