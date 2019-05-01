// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "../../common/plugins/PluginJsonConstants.h"
#include "DirectPluginProxy.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    DirectPluginProxy::DirectPluginProxy(
        const std::string& pluginFullPath) :
        _hModule(NULL),
        _pluginFullPath(pluginFullPath),
        PluginCreate(NULL),
        PluginGetHandlersInfo(NULL),
        PluginCreateRawHandler(NULL),
        PluginDestroyRawHandler(NULL)
    {}

    void DirectPluginProxy::Load()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        wstring win32FullFilePath = Utils::MultibyteToWide(_pluginFullPath.c_str());
        _hModule = LoadLibrary(win32FullFilePath.c_str());
        if (_hModule == NULL)
        {
            string msg = "Failed to load library: " + _pluginFullPath;
            throw DMException(DMSubsystem::Windows, GetLastError(), msg);
        }
        TRACELINEP(LoggingLevel::Verbose, "Successfully loaded: ", _pluginFullPath.c_str());

        // PluginCreateStr
        PluginCreate = (PluginCreatePtr)GetProcAddress(_hModule, PluginCreateStr);
        if (NULL == PluginCreate)
        {
            string msg = string("Failed to find entry point: ") + PluginCreateStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }

        // PluginGetHandlersInfoStr
        PluginGetHandlersInfo = (PluginGetHandlersInfoPtr)GetProcAddress(_hModule, PluginGetHandlersInfoStr);
        if (NULL == PluginGetHandlersInfo)
        {
            string msg = string("Failed to find entry point: ") + PluginGetHandlersInfoStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }

        // PluginCreateRawHandlerStr
        PluginCreateRawHandler = (PluginCreateRawHandlerPtr)GetProcAddress(_hModule, PluginCreateRawHandlerStr);
        if (NULL == PluginCreateRawHandler)
        {
            string msg = string("Failed to find entry point: ") + PluginCreateRawHandlerStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }

        // PluginDestroyRawHandlerStr
        PluginDestroyRawHandler = (PluginDestroyRawHandlerPtr)GetProcAddress(_hModule, PluginDestroyRawHandlerStr);
        if (NULL == PluginDestroyRawHandler)
        {
            string msg = string("Failed to find entry point: ") + PluginDestroyRawHandlerStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, msg);
        }

        // Initialize...
        PluginCreate();
    }

    void DirectPluginProxy::Unload()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        if (_hModule != NULL)
        {
            FreeLibrary(_hModule);
        }

        // Set all the function pointers to null
        PluginCreate = NULL;
        PluginGetHandlersInfo = NULL;
        PluginCreateRawHandler = NULL;
        PluginDestroyRawHandler = NULL;
    }

    std::vector<DMCommon::HandlerInfo> DirectPluginProxy::GetHandlersInfo() const
    {
        if (PluginGetHandlersInfo == nullptr)
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, "Failed to find entry point GetHandlersInfo.");
        }
        return PluginGetHandlersInfo();
    }

    std::shared_ptr<DMCommon::IRawHandler> DirectPluginProxy::CreateRawHandler(
        const std::string& id)
    {
        if (PluginCreateRawHandler == nullptr)
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, "Failed to find entry point PluginCreateRawHandler.");
        }
        return PluginCreateRawHandler(id);
    }

    void DirectPluginProxy::DestroyRawHandler(
        const std::string& id)
    {
        if (PluginDestroyRawHandler == nullptr)
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MISSING_ENTRY_POINT, "Failed to find entry point PluginDestroyRawHandler.");
        }
        return PluginDestroyRawHandler(id);
    }

}}}}
