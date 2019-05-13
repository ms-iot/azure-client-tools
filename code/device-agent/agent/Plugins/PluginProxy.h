// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <vector>
#include <map>
#include "../../common/DMInterfaces.h"
#include "../../common/plugins/PluginInterfaces.h"
#include "RawHandlerProxy.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class PluginProxy : public DMCommon::IPlugin
    {
    public:

        PluginProxy(const std::string& pluginFullPath, bool outOfProc, long keepAliveTime);
        ~PluginProxy();

        void Load();

        std::vector<DMCommon::HandlerInfo> GetHandlersInfo() const;

        // Raw Interfaces

        std::shared_ptr<DMCommon::IRawHandler> CreateRawHandler(
            const std::string& id);

        void DestroyRawHandler(
            const std::string& id);

        void Unload();

    private:
        bool _outOfProc;
        long _keepAliveTime;
        std::shared_ptr<DMCommon::IPluginBinaryProxy> _binaryProxy;
    };

}}}}
