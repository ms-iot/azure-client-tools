// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <windows.h>
#include <filesystem>
#include <TraceLoggingProvider.h>
#include "DMString.h"
#include "ETWLogger.h"

TRACELOGGING_DEFINE_PROVIDER(
    g_debugEventProvider,
    "AzureDMClient",
    // {e1688237-74f7-54f0-7ff7-9ba255fa157c}
    (0xe1688237, 0x74f7, 0x54f0, 0x7f, 0xf7, 0x9b, 0xa2, 0x55, 0xfa, 0x15, 0x7c));

using namespace std;

static const int WinEventLevelVerbose = 5;
static const int WinEventLevelInformation = 4;
static const int WinEventLevelWarning = 3;
static const int WinEventLevelError = 2;
static const int WinEventLevelCritical = 1;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    ETWLogger::ETWLogger()
    {
        TraceLoggingRegister(g_debugEventProvider);
    }

    ETWLogger::~ETWLogger()
    {
        TraceLoggingUnregister(g_debugEventProvider);
    }

    void ETWLogger::Log(const std::wstring& msg, LoggingLevel level)
    {
        switch (level)
        {
        case LoggingLevel::Verbose:
            TraceLoggingWrite(g_debugEventProvider, "LogMsgVerbose",
                TraceLoggingLevel(WinEventLevelVerbose),
                TraceLoggingWideString(msg.c_str(), "msg"));
            break;
        case LoggingLevel::Information:
            TraceLoggingWrite(g_debugEventProvider, "LogMsgInformation",
                TraceLoggingLevel(WinEventLevelInformation),
                TraceLoggingWideString(msg.c_str(), "msg"));
            break;
        case LoggingLevel::Warning:
            TraceLoggingWrite(g_debugEventProvider, "LogMsgWarning",
                TraceLoggingLevel(WinEventLevelWarning),
                TraceLoggingWideString(msg.c_str(), "msg"));
            break;
        case LoggingLevel::Error:
            TraceLoggingWrite(g_debugEventProvider, "LogMsgError",
                TraceLoggingLevel(WinEventLevelError),
                TraceLoggingWideString(msg.c_str(), "msg"));
            break;
        case LoggingLevel::Critical:
            TraceLoggingWrite(g_debugEventProvider, "LogMsgCritical",
                TraceLoggingLevel(WinEventLevelCritical),
                TraceLoggingWideString(msg.c_str(), "msg"));
            break;
        }
    }

    void ETWLogger::Log(const std::string& msg, LoggingLevel level)
    {
        wstring s = MultibyteToWide(msg.c_str());
        Log(s, level);
    }

}}}}
