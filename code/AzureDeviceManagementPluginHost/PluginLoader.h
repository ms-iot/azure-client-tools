// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

#include "..\AzureDeviceManagementCommon\DMInterfaces.h"
#include "..\AzureDeviceManagementCommon\Plugins\PluginInterfaces.h"

namespace Microsoft {namespace Azure { namespace DeviceManagement { namespace Client {
    class PluginLoader
    {
    public:
        PluginLoader(const std::wstring& pluginFullPath);

        void Load();

        DMCommon::PluginCreatePtr PluginCreate;

        DMCommon::PluginInvokePtr PluginInvoke;
        DMCommon::PluginDeleteBufferPtr PluginDeleteBuffer;

        DMCommon::PluginSetReverseInvokePtr PluginSetReverseInvoke;
        DMCommon::PluginSetReverseDeleteBufferPtr PluginSetReverseDeleteBuffer;

    private:
        HMODULE _hModule;
        std::wstring _pluginFullPath;
    };
}}}}
