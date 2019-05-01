// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

#include "../../common/DMInterfaces.h"
#include "../../common/plugins/PluginInterfaces.h"
#include "../../common/plugins/CrossBinaryResponse.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class OutOfProcPluginBinaryProxy :
        public DMCommon::IPluginBinaryProxy,
        public DMCommon::IPluginStateMonitor
    {
        struct HandlerState
        {
            bool started;
            Json::Value startParams;
            Json::Value updateConnectionParams;

            HandlerState() :
                started(false)
            {}
        };

    public:
        OutOfProcPluginBinaryProxy(const std::string& pluginFullPath, long keepAliveTime);

        void Load();

        Json::Value Invoke(
            const std::string& targetType,
            const std::string& targetId,
            const std::string& targetMethod,
            const Json::Value& targetParameters);

        void Unload();

        // IPluginStateMonitor
        void OnPluginLoaded();

    private:

        void SendCreatePluginRequest();

        void CacheHandlerState(
            const std::string& targetType,
            const std::string& targetId,
            const std::string& targetMethod,
            const Json::Value& targetParameters);

        void SendCachedHandlerState(
            const std::map<std::string, HandlerState>& handlersStates,
            const std::string& jsonTargetTypeName,
            const std::string& jsonCreateMethodName);

        std::shared_ptr<DMCommon::IPluginTransport> _transport;

        std::map<std::string, HandlerState> _rawHandlersStates;
    };
}}}}
