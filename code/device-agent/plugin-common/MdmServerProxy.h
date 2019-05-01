// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// ToDo: use common paths
#include "device-agent/common/plugins/PluginInterfaces.h"
#include "device-agent/common/plugins/CrossBinaryResponse.h"
#include "AgentBinaryProxy.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    class MdmServerProxy : public DMCommon::IMdmServer
    {
    public:
        MdmServerProxy(AgentBinaryProxy* agentBinaryProxy);

        std::string RunSyncML(const std::string& sid, const std::string& syncML);
    private:
        AgentBinaryProxy* _agentBinaryProxy;
    };

}}}}}
