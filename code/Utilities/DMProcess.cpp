// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <windows.h>
#include <Psapi.h>
#include "DMProcess.h"
#include "AutoCloseHandle.h"
#include "DMException.h"
#include "Logger.h"

static const int ERROR_PIPE_HAS_BEEN_ENDED = 109;

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

void Process::Launch(
    const std::wstring& commandString,
    unsigned long& returnCode,
    std::string& output)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    SECURITY_ATTRIBUTES securityAttributes;
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.bInheritHandle = TRUE;
    securityAttributes.lpSecurityDescriptor = NULL;

    AutoCloseHandle stdOutReadHandle;
    AutoCloseHandle stdOutWriteHandle;
    DWORD pipeBufferSize = 4096;

    if (!CreatePipe(stdOutReadHandle.GetAddress(), stdOutWriteHandle.GetAddress(), &securityAttributes, pipeBufferSize))
    {
        throw DMException(GetLastError());
    }

    if (!SetHandleInformation(stdOutReadHandle.Get(), HANDLE_FLAG_INHERIT, 0 /*flags*/))
    {
        throw DMException(GetLastError());
    }

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = stdOutWriteHandle.Get();
    siStartInfo.hStdOutput = stdOutWriteHandle.Get();
    siStartInfo.hStdInput = NULL;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcess(NULL,
        const_cast<wchar_t*>(commandString.c_str()), // command line 
        NULL,         // process security attributes 
        NULL,         // primary thread security attributes 
        TRUE,         // handles are inherited 
        0,            // creation flags 
        NULL,         // use parent's environment 
        NULL,         // use parent's current directory 
        &siStartInfo, // STARTUPINFO pointer 
        &piProcInfo)) // receives PROCESS_INFORMATION
    {
        throw DMException(GetLastError());
    }
    TRACELINE(LoggingLevel::Verbose, "Child process has been launched.");

    bool doneWriting = false;
    while (!doneWriting)
    {
        // Let the child process run for 1 second, and then check if there is anything to read...
        DWORD waitStatus = WaitForSingleObject(piProcInfo.hProcess, 1000);
        if (waitStatus == WAIT_OBJECT_0)
        {
            TRACELINE(LoggingLevel::Verbose, "Child process has exited.");
            if (!GetExitCodeProcess(piProcInfo.hProcess, &returnCode))
            {
                // We just ignore - something wrong with the API took place.
                TRACEP(LoggingLevel::Verbose, "Warning: Failed to get process exist code. GetLastError() = ", GetLastError());
            }
            CloseHandle(piProcInfo.hProcess);
            CloseHandle(piProcInfo.hThread);

            // Child process has exited, no more writing will take place.
            // Without closing the write channel, the ReadFile will keep waiting.
            doneWriting = true;
            stdOutWriteHandle.Close();
        }
        else
        {
            TRACELINE(LoggingLevel::Verbose, "Child process is still running...");
        }

        DWORD bytesAvailable = 0;
        if (PeekNamedPipe(stdOutReadHandle.Get(), NULL, 0, NULL, &bytesAvailable, NULL))
        {
            if (bytesAvailable > 0)
            {
                DWORD readByteCount = 0;
                vector<char> readBuffer(bytesAvailable + 1);
                if (ReadFile(stdOutReadHandle.Get(), readBuffer.data(), static_cast<unsigned int>(readBuffer.size()) - 1, &readByteCount, NULL) || readByteCount == 0)
                {
                    readBuffer[readByteCount] = '\0';
                    output += readBuffer.data();
                }
            }
        }
        else
        {
            DWORD retCode = GetLastError();
            if (ERROR_PIPE_HAS_BEEN_ENDED != retCode)
            {
                printf("error code = %d\n", retCode);
            }
            break;
        }
    }

    TRACEP(LoggingLevel::Verbose, "Command return Code: ", returnCode);
    TRACEP(LoggingLevel::Verbose, "Command output : ", output.c_str());
}

wstring Process::GetProcessExePath(
    DWORD processID)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    AutoCloseHandle processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (NULL == processHandle.Get())
    {
        throw DMException(GetLastError(), "Failed to open process.");
    }

    HMODULE moduleHandle = NULL;   // do not call CloseHandle() on handles returned by EnumProcessModules() (see MSDN).
    DWORD neededBytes = 0;
    wchar_t exePath[MAX_PATH] = L"<unknown>";
    if (!EnumProcessModules(processHandle.Get(), &moduleHandle, sizeof(moduleHandle), &neededBytes))
    {
        throw DMException(GetLastError(), "Failed to enumerate process modules.");
    }

    if (0 == GetModuleFileNameEx(processHandle.Get(), moduleHandle, exePath, sizeof(exePath) / sizeof(wchar_t)))
    {
        throw DMException(GetLastError(), "Failed to get the module file name.");
    }

    return wstring(exePath);
}

bool Process::IsProcessRunning(
    const wstring& processName)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    bool running = false;
    TRACEP(LoggingLevel::Verbose, L"Checking: ", processName.c_str());

    DWORD processHandles[1024];
    DWORD bytesNeeded = 0;
    if (!EnumProcesses(processHandles, sizeof(processHandles), &bytesNeeded))
    {
        throw DMException(GetLastError(), "Failed to enumerate running processes.");
    }

    DWORD processCount = bytesNeeded / sizeof(DWORD);
    for (DWORD i = 0; i < processCount; i++)
    {
        if (processHandles[i] == 0)
        {
            continue;
        }
        wstring exePath = GetProcessExePath(processHandles[i]);
        TRACEP(LoggingLevel::Verbose, L"Found Process: ", exePath.c_str());

        if (Utils::Contains(exePath, processName))
        {
            TRACELINE(LoggingLevel::Verbose, L"process is running!");
            running = true;
            break;
        }
    }

    return running;
}

}}}}