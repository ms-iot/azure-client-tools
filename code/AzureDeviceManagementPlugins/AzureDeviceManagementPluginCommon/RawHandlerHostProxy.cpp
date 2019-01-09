// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "stdafx.h"
#include "RawHandlerHostProxy.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    RawHandlerHostProxy::RawHandlerHostProxy(AgentBinaryProxy* agentBinaryProxy) :
        _agentBinaryProxy(agentBinaryProxy)
    {}

    void RawHandlerHostProxy::Report(const std::string& id, DeploymentStatus deploymentStatus, const Json::Value& value)
    {
        // Build Request
        Json::Value jsonValue(Json::ValueType::objectValue);
        jsonValue[JsonRawReportId] = Json::Value(id);
        jsonValue[JsonRawReportDeploymentStatus] = Json::Value(DeploymentStatusToString(deploymentStatus));
        jsonValue[JsonRawReportValue] = Json::Value(value);

        // Invoke
        string requestString = CrossBinaryRequest::BuildRequest(JsonTargetTypePluginHost, "", JsonRawReport, jsonValue);

        // Output
        _agentBinaryProxy->ReverseInvoke(requestString);
    }

}}}}}
