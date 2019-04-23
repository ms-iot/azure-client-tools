// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

#include "../../common/DMInterfaces.h"
#include "../../common/plugins/PluginInterfaces.h"
#include "../../common/plugins/CrossBinaryResponse.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class PluginBinaryProxy : public DMCommon::IPluginBinaryProxy
    {
    public:
        PluginBinaryProxy(const std::string& pluginFullPath);

        void Load();

        Json::Value Invoke(
            const std::string& targetType,
            const std::string& targetId,
            const std::string& targetMethod,
            const Json::Value& targetParameters);

        void Unload();

    private:

        DMCommon::PluginCreatePtr PluginCreate;

        DMCommon::PluginInvokePtr PluginInvoke;
        DMCommon::PluginDeleteBufferPtr PluginDeleteBuffer;

        DMCommon::PluginSetReverseInvokePtr PluginSetReverseInvoke;
        DMCommon::PluginSetReverseDeleteBufferPtr PluginSetReverseDeleteBuffer;

        HMODULE _hModule;
        std::string _pluginFullPath;
    };

}}}}
