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
            throw DMException(DMSubsystem::Windows, GetLastError(), "GetTempPath failed.");
        }

        return folder;
    }

    wstring GetTempFileW(const wstring& prefix)
    {
        wchar_t szTempFileName[MAX_PATH];
        wchar_t path[MAX_PATH];

        if (GetTempPath(MAX_PATH, path) == 0)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: failed to retrieve temp folder.");
        }
        else if (GetTempFileName(path, prefix.c_str(), 1, szTempFileName) == 0)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: failed to create a temp filename.");
        }

        return wstring(szTempFileName);
    }

    wstring GetSystemRootFolderW()
    {
        UINT size = GetSystemDirectory(0, 0);

        vector<wchar_t> buffer(size);
        if (size != GetSystemDirectory(buffer.data(), static_cast<unsigned int>(buffer.size())) + 1)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: failed to retrieve system folder.");
        }
        return wstring(buffer.data());
    }

    string GetProcessPath()
    {
        wchar_t moduleFileName[_MAX_PATH];
        DWORD retCode = GetModuleFileName(NULL, moduleFileName, _MAX_PATH);
        if (retCode == _MAX_PATH && ERROR_INSUFFICIENT_BUFFER == GetLastError())
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: failed to retrieve process file name.");
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

    void EnsureFolderExists(const string& folder)
    {
        vector<string> tokens;
        SplitString(folder, '\\', tokens);
        size_t index = 0;
        string path = "";
        for (const string& s : tokens)
        {
            if (index == 0)
            {
                path += s;
            }
            else
            {
                path += "\\";
                path += s;
                if (ERROR_SUCCESS != CreateDirectoryA(path.c_str(), NULL))
                {
                    if (ERROR_ALREADY_EXISTS != GetLastError())
                    {
                        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to create new directory");
                    }
                }
            }
            ++index;
        }
    }

    vector<string> GetFileSystemObjectNames(const wstring& path, file_type type)
    {

        TRACELINEP(LoggingLevel::Verbose, "Scanning: ", WideToMultibyte(path.c_str()).c_str());

        vector<string> vector;
        for (const directory_entry& dirEntry : directory_iterator(path))
        {
            TRACELINEP(LoggingLevel::Verbose, "Found: ", WideToMultibyte(dirEntry.path().c_str()).c_str());
            if (dirEntry.status().type() != type)
            {
                continue;
            }

            string folderName = WideToMultibyte(dirEntry.path().filename().c_str());
            vector.push_back(folderName);

            TRACELINEP(LoggingLevel::Verbose, "  Picked: ", folderName.c_str());
        }
        return vector;
    }
}}}}