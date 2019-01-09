// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AgentBinaryProxy.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    class RawHandlerHostProxy : public DMCommon::IRawHandlerHost
    {
    public:
        RawHandlerHostProxy(AgentBinaryProxy* agentBinaryProxy);

        void Report(
            const std::string& id,
            DMCommon::DeploymentStatus deploymentStatus,
            const Json::Value& value);

    private:
        AgentBinaryProxy* _agentBinaryProxy;
    };

}}}}}
