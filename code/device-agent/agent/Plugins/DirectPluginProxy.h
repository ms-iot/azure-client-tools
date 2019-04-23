// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <vector>
#include <map>
#include "../../common/DMInterfaces.h"
#include "RawHandlerProxy.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class DirectPluginProxy : public DMCommon::IPlugin
    {
    public:
        DirectPluginProxy(
            const std::string& pluginFullPath);

        void Load();

        std::vector<DMCommon::HandlerInfo> GetHandlersInfo() const;

        // Raw Interfaces

        std::shared_ptr<DMCommon::IRawHandler> CreateRawHandler(
            const std::string& id);

        void DestroyRawHandler(
            const std::string& id);

        void Unload();

    private:
        DMCommon::PluginCreatePtr PluginCreate;
        DMCommon::PluginGetHandlersInfoPtr PluginGetHandlersInfo;

        DMCommon::PluginCreateRawHandlerPtr PluginCreateRawHandler;
        DMCommon::PluginDestroyRawHandlerPtr PluginDestroyRawHandler;

        HMODULE _hModule;
        std::string _pluginFullPath;
    };

}}}}
