// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "AutoCloseHandle.h"
#include "DMException.h"
#include "Logger.h"
#include "Shell.h"

// SHGetFolderPath
#include "Shlobj.h"
// CreateToolhelp32Snapshot, etc
#include <tlhelp32.h>

#define SihostExe L"sihost.exe" 

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    HANDLE Shell::GetShellUserToken(unsigned int attemptCount, unsigned int attemptDelay)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        do
        {
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);

            AutoCloseHandle snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
            if (!snapshot.Get())
            {
                throw DMException(DMSubsystem::Windows, GetLastError(), "CreateToolhelp32Snapshot failed to create snapshot of running processes.");
            }

            if (Process32First(snapshot.Get(), &entry) == TRUE)
            {
                while (Process32Next(snapshot.Get(), &entry) == TRUE)
                {
                    if (_wcsicmp(entry.szExeFile, SihostExe) == 0)
                    {
                        AutoCloseHandle processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                        if (processHandle.Get() == INVALID_HANDLE_VALUE) continue;

                        HANDLE processTokenHandle;
                        auto error = OpenProcessToken(processHandle.Get(), TOKEN_ALL_ACCESS, &processTokenHandle);
                        if (error == 0)
                        {
                            TRACELINEP(LoggingLevel::Error, L"OpenProcessToken failed. Code: ", GetLastError());
                            continue;
                        }

                        return processTokenHandle;
                    }
                }
            }

            --attemptCount;
            if (attemptCount == 0)
            {
                break;
            }

            TRACELINE(LoggingLevel::Verbose, "GetShellUserInfo: no user process found. Retrying...");
            ::Sleep(attemptDelay);

        } while (true);

        throw DMException(DMSubsystem::Windows, DM_SHELL_ERROR_NO_USER_PROCESS_FOUND, "GetShellUserInfo: no user process found.");
    }

    std::wstring Shell::GetDmUserFolder()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // this works on IoT Core and IoT Enterprise (not IoT Enterprise 
        // Mobile ... SHGetFolderPath not implemented there)
        std::wstring folder(L"");

        AutoCloseHandle processTokenHandle = Shell::GetShellUserToken(10 /*attempts*/, 2000 /*2 sec*/); // The SiHostExe might not have started.

        WCHAR szPath[MAX_PATH];
        HRESULT hr = SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, processTokenHandle.Get(), 0, szPath);
        if (SUCCEEDED(hr))
        {
            folder = szPath;
            folder += L"\\Temp\\IotDm\\";
            if (!CreateDirectory(folder.c_str(), nullptr))
            {
                DWORD lastError = GetLastError();
                if (ERROR_ALREADY_EXISTS != lastError)
                {
                    TRACELINEP(LoggingLevel::Error, L"CreateDirectory failed. Code: ", lastError);
                    throw DMException(DMSubsystem::Windows, lastError, "Failed to create directory");
                }
            }
        }
        else
        {
            TRACELINEP(LoggingLevel::Verbose, "SHGetFolderPath failed. Code: ", hr);
            throw DMException(DMSubsystem::Windows, hr, "Failed to get local appdata path for user");
        }
        return folder;
    }
}}}}