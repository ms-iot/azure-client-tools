// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "stdafx.h"
#include "RawHandlerHostProxy.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    RawHandlerHostProxy::RawHandlerHostProxy(
        AgentBinaryProxy* agentBinaryProxy,
        shared_ptr<IMdmServer> mdmServer) :
        _agentBinaryProxy(agentBinaryProxy),
        _mdmServer(mdmServer)
    {}

    std::shared_ptr<IMdmServer> RawHandlerHostProxy::GetMdmServer() const
    {
        return _mdmServer;
    }

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

    void RawHandlerHostProxy::SendEvent(const std::string& id, const std::string& eventName, const Json::Value& value)
    {
        // Build Request
        Json::Value jsonValue(Json::ValueType::objectValue);
        jsonValue[JsonRawSendEventInterfaceName] = Json::Value(id);
        jsonValue[JsonRawSendEventEventName] = Json::Value(eventName);
        jsonValue[JsonRawSendEventMessageData] = Json::Value(value);

        // Invoke
        string requestString = CrossBinaryRequest::BuildRequest(JsonTargetTypePluginHost, "", JsonRawSendEvent, jsonValue);

        // Output
        _agentBinaryProxy->ReverseInvoke(requestString);
    }

}}}}}