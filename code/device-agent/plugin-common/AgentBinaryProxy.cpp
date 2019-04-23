// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "stdafx.h"
#include "AgentBinaryProxy.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    AgentBinaryProxy::AgentBinaryProxy() :
        AgentReverseInvoke(nullptr),
        AgentReverseDeleteBuffer(nullptr)
    {}

    void AgentBinaryProxy::SetReverseInvoke(PluginReverseInvokePtr ReverseInvoke_)
    {
        AgentReverseInvoke = ReverseInvoke_;
    }

    void AgentBinaryProxy::SetReverseDeleteBuffer(PluginReverseDeleteBufferPtr ReverseDeleteBuffer_)
    {
        AgentReverseDeleteBuffer = ReverseDeleteBuffer_;
    }

    Json::Value AgentBinaryProxy::ReverseInvoke(
        const std::string& jsonInputString)
    {
        char* responseBuffer;
        int retCode = AgentReverseInvoke(jsonInputString.c_str(), &responseBuffer);
        if (retCode != DM_ERROR_SUCCESS)
        {
            throw DMException(DMSubsystem::DeviceAgent, retCode, "AgentReverseInvoke returned error.");
        }

        // If AgentReverseInvoke() did not create a response, fail right away.
        if (responseBuffer == NULL)
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_INVALID_OUTPUT_BUFFER, "AgentReverseInvoke returned invalid buffer.");
        }

        // Copy to local heap.
        string responseString = responseBuffer;

        // Delete from plug-in heap.
        AgentReverseDeleteBuffer(responseBuffer);

        // Translates failures to exceptions.
        return CrossBinaryResponse::GetPayloadFromJsonString(responseString);
    }

}}}}}
