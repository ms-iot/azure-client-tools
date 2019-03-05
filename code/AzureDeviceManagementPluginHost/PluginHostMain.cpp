// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <iostream>
#include <string>
#include "DMConstants.h"
#include "..\Utilities\Utils.h"
#include "PluginLoader.h"
#include "PluginHostStub.h"
#include "..\AzureDeviceManagementCommon\Plugins\PluginNamedPipeTransport.h"

using namespace std;
using namespace DMCommon;
using namespace DMUtils;
using namespace Microsoft::Azure::DeviceManagement::Client;

HANDLE g_shutdownEvent;

[Platform::MTAThread]
int wmain(int argc, wchar_t *argv[])
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Expected invocation format
    // DMClientPluginHost -pluginPath <path> -clientPipeName <name> -pluginPipeName <name>
    const int numArguments = 7;
    if (argc < numArguments)
    {
        TRACELINE(LoggingLevel::Verbose, L"Invalid number of arguments to plugin host. Exiting");
        return -1;
    }

    std::wstring pluginPath;
    std::wstring clientPipeName;
    std::wstring pluginPipeName;
    std::wstring logsPath;

    for (int i = 1; i < argc - 1; i += 2)
    {
        if (_wcsicmp(argv[i], L"-pluginPath") == 0)
        {
            pluginPath = argv[i + 1];
        }
        else if (_wcsicmp(argv[i], L"-clientPipeName") == 0)
        {
            clientPipeName = argv[i + 1];
        }
        else if (_wcsicmp(argv[i], L"-pluginPipeName") == 0)
        {
            pluginPipeName = argv[i + 1];
        }
        else if (_wcsicmp(argv[i], L"-logsPath") == 0)
        {
            logsPath = argv[i + 1];
        }
    }

    if (logsPath.size() != 0)
    {
        gLogger.SetLogFilePath(logsPath, wstring(L"PluginHost_") + to_wstring(GetCurrentProcessId()) + L"_");
    }

    if (pluginPath.empty() || clientPipeName.empty() || pluginPipeName.empty())
    {
        TRACELINE(LoggingLevel::Verbose, "Invalid parameters passed to plugin host. Exiting");
        return -1;
    }

    try
    {
        // Load the plugin Binary 
        PluginLoader _pluginBinary(pluginPath.c_str());
        _pluginBinary.Load();

        shared_ptr<DMCommon::IPluginTransport> transport = make_shared<DMCommon::PluginNamedPipeTransport>(false, L"", 0, clientPipeName, pluginPipeName);

        // Get the shutdown event. Main thread will wait on this event
        g_shutdownEvent = transport->GetShutdownNotificationEvent();

        // Set plugin interfaces
        transport->SetPluginInterface(_pluginBinary.PluginCreate, _pluginBinary.PluginInvoke, _pluginBinary.PluginDeleteBuffer);

        // Setup PluginHostStub
        PluginHostStub::SetPluginTransport(transport);
        _pluginBinary.PluginSetReverseInvoke(PluginHostStub::ReverseInvoke);
        _pluginBinary.PluginSetReverseDeleteBuffer(PluginHostStub::ReverseDeleteBuffer);

        // Initialize the transport
        transport->Initialize();
        TRACELINE(LoggingLevel::Verbose, "Transport initialization successful");

        // Wait for shutdown event
        WaitForSingleObject(g_shutdownEvent, INFINITE);

        CloseHandle(g_shutdownEvent);
        g_shutdownEvent = INVALID_HANDLE_VALUE;
    }
    catch (const DMException& dmEx)
    {
        string msg;
        msg += "Fatal Error: ";
        msg += to_string(dmEx.Code());
        msg += " : ";
        msg += dmEx.Message();

        TRACELINE(LoggingLevel::Error, msg.c_str());
    }
    catch (const exception& ex)
    {
        string msg;
        msg += "Fatal Error: ";
        msg += ex.what();

        TRACELINE(LoggingLevel::Error, msg.c_str());
    }
    catch (...)
    {
        TRACELINE(LoggingLevel::Error, "Fatal Error: Unkown Exception.");
    }

    TRACELINE(LoggingLevel::Verbose, "Exiting wmain.");
    return 0;
}

