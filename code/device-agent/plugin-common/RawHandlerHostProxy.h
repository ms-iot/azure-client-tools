// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AgentBinaryProxy.h"
#include "device-agent/common/plugins/PluginInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    class RawHandlerHostProxy : public DMCommon::IRawHandlerHost
    {
    public:
        RawHandlerHostProxy(
            AgentBinaryProxy* agentBinaryProxy,
            std::shared_ptr<DMCommon::IMdmServer> mdmServer);

        std::shared_ptr<DMCommon::IMdmServer> GetMdmServer() const;

        void Report(
            const std::string& id,
            DMCommon::DeploymentStatus deploymentStatus,
            const Json::Value& value);

        void SendEvent(
            const std::string& id,
            const std::string& eventName,
            const Json::Value& value);

    private:
        AgentBinaryProxy* _agentBinaryProxy;
        std::shared_ptr<DMCommon::IMdmServer> _mdmServer;
    };

}}}}}
