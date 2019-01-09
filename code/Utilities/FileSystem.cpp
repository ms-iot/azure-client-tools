// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <windows.h>
#include "DMException.h"
#include "FileSystem.h"
#include "DMString.h"

using namespace std;
using namespace std::experimental::filesystem;
using namespace std::experimental::filesystem::v1;

namespace DMUtils = Microsoft::Azure::DeviceManagement::Utils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    wstring GetTempFolder()
    {
        WCHAR szPath[MAX_PATH];
        wstring folder(L"");

        DWORD result = GetTempPath(MAX_PATH, szPath);
        if (result)
        {
            folder = szPath;
        }
        else
        {
            throw DMException(GetLastError(), "GetTempPath failed.");
        }

        return folder;
    }

    wstring GetSystemRootFolderW()
    {
        UINT size = GetSystemDirectory(0, 0);

        vector<wchar_t> buffer(size);
        if (size != GetSystemDirectory(buffer.data(), static_cast<unsigned int>(buffer.size())) + 1)
        {
            throw DMException(GetLastError(), "Error: failed to retrieve system folder.");
        }
        return wstring(buffer.data());
    }

    string GetProcessPath()
    {
        wchar_t moduleFileName[_MAX_PATH];
        DWORD retCode = GetModuleFileName(NULL, moduleFileName, _MAX_PATH);
        if (retCode == _MAX_PATH && ERROR_INSUFFICIENT_BUFFER == GetLastError())
        {
            throw DMException(retCode, "Error: failed to retrieve process file name.");
        }

        path p(moduleFileName);
        
        return DMUtils::WideToMultibyte(p.parent_path().c_str());
    }

    string MakePathAbsolute(const string& root, const string& fileName)
    {
        string absolutePath = fileName;

        path p(fileName);
        if (p.is_relative())
        {
            absolutePath = root + "\\" + fileName;
        }

        return absolutePath;
    }

}}}}

