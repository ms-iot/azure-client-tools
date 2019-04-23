// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginLoader.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    PluginLoader::PluginLoader(const std::wstring& pluginFullPath) :
        _hModule(NULL),
        _pluginFullPath(pluginFullPath),
        PluginCreate(NULL),
        PluginInvoke(NULL),
        PluginDeleteBuffer(NULL),
        PluginSetReverseInvoke(NULL),
        PluginSetReverseDeleteBuffer(NULL)
    {}

    void PluginLoader::Load()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        string path = Utils::WideToMultibyte(_pluginFullPath.c_str());
        _hModule = LoadLibrary(_pluginFullPath.c_str());
        if (_hModule == NULL)
        {
            string msg = "Failed to load library: " + path;
            throw DMException(DMSubsystem::Windows, GetLastError(), msg);
        }
        TRACELINEP(LoggingLevel::Verbose, "Successfully loaded: ", path.c_str());

        // PluginCreateStr
        PluginCreate = (PluginCreatePtr)GetProcAddress(_hModule, PluginCreateStr);
        if (NULL == PluginCreate)
        {
            string msg = string("Failed to find entry point: ") + PluginCreateStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }

        // PluginInvokeStr
        PluginInvoke = (PluginInvokePtr)GetProcAddress(_hModule, PluginInvokeStr);
        if (NULL == PluginInvoke)
        {
            string msg = string("Failed to find entry point: ") + PluginInvokeStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }

        // PluginDeleteStr
        PluginDeleteBuffer = (PluginDeleteBufferPtr)GetProcAddress(_hModule, PluginDeleteBufferStr);
        if (NULL == PluginDeleteBuffer)
        {
            string msg = string("Failed to find entry point: ") + PluginDeleteBufferStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }

        // PluginSetReverseInvokeStr
        PluginSetReverseInvoke = (PluginSetReverseInvokePtr)GetProcAddress(_hModule, PluginSetReverseInvokeStr);
        if (NULL == PluginSetReverseInvoke)
        {
            string msg = string("Failed to find entry point: ") + PluginSetReverseInvokeStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }

        // PluginSetReverseDeleteBufferStr
        PluginSetReverseDeleteBuffer = (PluginSetReverseDeleteBufferPtr)GetProcAddress(_hModule, PluginSetReverseDeleteBufferStr);
        if (NULL == PluginSetReverseDeleteBuffer)
        {
            string msg = string("Failed to find entry point: ") + PluginSetReverseDeleteBufferStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }
    }
}}}}
