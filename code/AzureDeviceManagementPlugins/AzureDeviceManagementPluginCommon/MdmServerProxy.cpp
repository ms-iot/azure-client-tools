// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "stdafx.h"
#include "MdmServerProxy.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

MdmServerProxy::MdmServerProxy(AgentBinaryProxy* agentBinaryProxy) :
    _agentBinaryProxy(agentBinaryProxy)
{}

string MdmServerProxy::RunSyncML(const std::string& sid, const std::string& syncML)
{
    // Build Request
    string syncMLBase64 = StringToBase64(syncML);   // Encode to escape syncml and make sure it's json compliant.

    Json::Value jsonValue(Json::ValueType::objectValue);
    jsonValue[JsonRawRunSyncMLSid] = Json::Value(sid);
    jsonValue[JsonRawRunSyncMLInput] = syncMLBase64;

    string requestString = CrossBinaryRequest::BuildRequest(JsonTargetTypeMdmServer, "", JsonRawRunSyncML, jsonValue);

    // Invoke
    Json::Value payload = _agentBinaryProxy->ReverseInvoke(requestString);

    // Output
    string outSyncMLBase64 = payload[JsonRawRunSyncMLOutput].asString();

    return Base64ToString(outSyncMLBase64);     // Decode result back to its original state.
}

}}}}}
