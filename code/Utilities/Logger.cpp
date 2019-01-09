// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <iostream> 
#include <iomanip>
#include <filesystem>

#include "DMString.h"
#include "Logger.h"
#include "ETWLogger.h"

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    ETWLogger gETWLogger;

    Logger::Logger() :
        _console(false)
    {
        TRACELINE(LoggingLevel::Verbose, "----New Session----------------------------------------------------------------");
    }

    void Logger::SetLogFilePath(const wstring& logFilePath, const std::wstring& prefix)
    {
        SYSTEMTIME systemTime;
        GetLocalTime(&systemTime);

        // format time
        basic_ostringstream<wchar_t> formattedTime;
        formattedTime 
            << setw(4) << setfill(L'0') << (int)systemTime.wYear
            << L'_' << setw(2) << setfill(L'0') << (int)systemTime.wMonth
            << L'_' << setw(2) << setfill(L'0') << (int)systemTime.wDay
            << L'_' << setw(2) << setfill(L'0') << (int)systemTime.wHour
            << L'_' << setw(2) << setfill(L'0') << (int)systemTime.wMinute
            << L'_' << setw(2) << setfill(L'0') << (int)systemTime.wSecond;

        wstring logFileName;
        logFileName += L"AzureDM.";
        logFileName += prefix;
        logFileName += L".";
        logFileName += formattedTime.str();
        logFileName += L".log";

        _logFileName = logFilePath + L"\\" + logFileName;
    }

    void Logger::EnableConsole(bool enable)
    {
        _console = enable;
    }

    void Logger::Log(LoggingLevel level, bool newLine, const char* msg)
    {
        wstring m = MultibyteToWide(msg);
        Log(level, newLine, m.c_str());
    }

    void Logger::Log(LoggingLevel level, bool newLine, const wchar_t* msg)
    {
        SYSTEMTIME systemTime;
        GetLocalTime(&systemTime);

        // format time
        basic_ostringstream<wchar_t> formattedTime;
        formattedTime << setw(2) << setfill(L'0') << (int)(systemTime.wHour > 12 ? (systemTime.wHour - 12) : systemTime.wHour)
            << L'-' << setw(2) << setfill(L'0') << (int)systemTime.wMinute
            << L'-' << setw(2) << setfill(L'0') << (int)systemTime.wSecond;

        // format process id
        basic_ostringstream<wchar_t> formattedProcessId;
        formattedProcessId << setw(8) << setfill(L'0') << GetProcessId(GetCurrentProcess());

        // format thread id
        basic_ostringstream<wchar_t> formattedThreadId;
        formattedThreadId << setw(8) << setfill(L'0') << GetThreadId(GetCurrentThread());

        // build message
        wstring messageWithTime = formattedTime.str() + L" "
            + (systemTime.wHour >= 12 ? L"PM " : L"AM ")
            + L"[" + formattedProcessId.str() + L"] "
            + L"[" + formattedThreadId.str() + L"] "
            + msg
            + (newLine ? L"\n" : L"");

        OutputDebugString(messageWithTime.c_str());

        // share...
        if (_console)
        {
            lock_guard<mutex> guard(_mutex);
            wcout << messageWithTime;
        }

        if (_logFileName.size() != 0)
        {
            lock_guard<mutex> guard(_mutex);

            wofstream logFile(_logFileName.c_str(), std::ofstream::out | std::ofstream::app);
            if (logFile)
            {
                logFile << messageWithTime.c_str();
                logFile.close();
            }
        }

        gETWLogger.Log(msg, level);
    }

    void Logger::Log(LoggingLevel level, bool newLine, const char*  msg, const char* param)
    {
        wstring m = MultibyteToWide(msg);
        wstring p = MultibyteToWide(param);
        Log<const wchar_t*>(level, newLine, m.c_str(), p.c_str());
    }

    void Logger::Log(LoggingLevel level, bool newLine, const char* msg, int param)
    {
        wstring m = MultibyteToWide(msg);
        Log<int>(level, newLine, m.c_str(), param);
    }

}}}}
