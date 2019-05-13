// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "OutOfProcPluginBinaryProxy.h"
#include "../../common/plugins/PluginHelpers.h"
#include "../../common/plugins/CrossBinaryRequest.h"
#include "../../common/plugins/PluginConstants.h"
#include "AgentStub.h"
#include "../../common/plugins/PluginJsonConstants.h"
#include "../../common/plugins/PluginNamedPipeAgentTransport.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

OutOfProcPluginBinaryProxy::OutOfProcPluginBinaryProxy(const std::string& pluginFullPath, long keepAliveTime)
{
    std::wstring widePluginPath = Utils::MultibyteToWide(pluginFullPath.c_str());
    _transport = shared_ptr<DMCommon::PluginNamedPipeAgentTransport>(new DMCommon::PluginNamedPipeAgentTransport(widePluginPath, keepAliveTime, L"", L"", this));
}

OutOfProcPluginBinaryProxy::~OutOfProcPluginBinaryProxy()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
}

void OutOfProcPluginBinaryProxy::Load()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Create named pipes
    // Create host process
    _transport->Initialize();

    _transport->SetClientInterface(AgentStub::ReverseInvoke, AgentStub::ReverseDeleteBuffer);
}

void OutOfProcPluginBinaryProxy::Unload()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Close the transport
    _transport->CloseTransport();
}

void OutOfProcPluginBinaryProxy::SendCreatePluginRequest()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    std::shared_ptr<Message> createRequest = make_shared<Message>();
    createRequest->messageType = Request;
    createRequest->callType = PluginCreateCall;
    createRequest->errorCode = DM_ERROR_SUCCESS;
    createRequest->SetData(PLUGIN_CREATE_REQUEST, strlen(PLUGIN_CREATE_REQUEST) + 1);

    std::shared_ptr<Message> createResponse = _transport->SendAndGetResponse(createRequest);
    if (createResponse->errorCode != DM_ERROR_SUCCESS)
    {
        throw DMException(DMSubsystem::DeviceAgent, createResponse->errorCode, "PluginCreate returned error.");
    }
}

void OutOfProcPluginBinaryProxy::CacheHandlerState(
    const std::string& targetType,
    const std::string& targetId,
    const std::string& targetMethod,
    const Json::Value& targetParameters)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    map<string, HandlerState>* handlersStates = nullptr;

    if (targetType.compare(JsonTargetTypeRaw) == 0 ||
        targetMethod.compare(JsonHostCreateRawHandler) == 0)
    {
        handlersStates = &_rawHandlersStates;
    }

    if (targetType.compare(JsonTargetTypeHost) == 0)
    {
        if (targetMethod.compare(JsonHostCreateRawHandler) == 0)
        {
            string handlerId = targetParameters[JsonHostCreateHandlerId].asCString();
            if (handlersStates->find(handlerId) == handlersStates->cend())
            {
                // Cache Create()...
                (*handlersStates)[handlerId] = HandlerState();
            }
        }
    }
    else
    {
        if (targetMethod.compare(JsonHandlerStart) == 0)
        {
            // Cache Start()...
            (*handlersStates)[targetId].started = true;
            (*handlersStates)[targetId].startParams = targetParameters;
        }
        else if (targetMethod.compare(JsonHandlerStop) == 0)
        {
            // Cache Stop()...
            (*handlersStates)[targetId].started = false;
        }
        else if (targetMethod.compare(JsonHandlerOnConnectionStatusChanged) == 0)
        {
            // Cache OnConnectionStatusChanged()...
            (*handlersStates)[targetId].updateConnectionParams = targetParameters;
        }
    }
}

void OutOfProcPluginBinaryProxy::SendCachedHandlerState(
    const map<string, HandlerState>& handlersStates,
    const string& jsonTargetTypeName,
    const string& jsonCreateMethodName)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    for (const pair<std::string, HandlerState>& p : handlersStates)
    {
        const string& handlerId = p.first;
        const HandlerState& handlerState = p.second;

        Json::Value createParameters(Json::ValueType::objectValue);
        createParameters[JsonHostCreateHandlerId] = Json::Value(handlerId);

        // Create the handler...
        Invoke(JsonTargetTypeHost, "", jsonCreateMethodName, createParameters);

        if (handlerState.started)
        {
            // Start the handler...
            Invoke(jsonTargetTypeName, handlerId, JsonHandlerStart, handlerState.startParams);
        }
        else
        {
            // Stop the handler...
            Invoke(jsonTargetTypeName, handlerId, JsonHandlerStop, handlerState.startParams);
        }

        // Update the connection status...
        Invoke(jsonTargetTypeName, handlerId, JsonHandlerOnConnectionStatusChanged, handlerState.updateConnectionParams);
    }
}

Json::Value OutOfProcPluginBinaryProxy::Invoke(
    const std::string& targetType,
    const std::string& targetId,
    const std::string& targetMethod,
    const Json::Value& targetParameters)
{
    if (_transport == nullptr)
    {
        throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_UNINITIALIZED_TRANSPORT, "Transport is not initialized");
    }

    CacheHandlerState(targetType, targetId, targetMethod, targetParameters);

    string request = CrossBinaryRequest::BuildRequest(targetType, targetId, targetMethod, targetParameters);

    // Send invoke request
    std::shared_ptr<Message> invokeRequest = make_shared<Message>();
    invokeRequest->messageType = Request;
    invokeRequest->callType = PluginInvokeCall;
    invokeRequest->errorCode = DM_ERROR_SUCCESS;
    invokeRequest->SetData(request.c_str(), request.size() + 1);

    std::shared_ptr<Message> invokeResponse = _transport->SendAndGetResponse(invokeRequest);
    if (invokeResponse->errorCode != DM_ERROR_SUCCESS)
    {
        throw DMException(DMSubsystem::DeviceAgent, invokeResponse->errorCode, "PluginInvoke returned error.");
    }

    if (strlen(invokeResponse->Payload()) == 0)
    {
        throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_INVALID_OUTPUT_BUFFER, "PluginInvoke returned invalid buffer.");
    }

    // Copy to local heap.
    string responseString = invokeResponse->Payload();

    // Translates failures to exceptions.
    return CrossBinaryResponse::GetPayloadFromJsonString(responseString);
}

void OutOfProcPluginBinaryProxy::OnPluginLoaded()
{
    SendCreatePluginRequest();

    // Create handler
    // Call Start
    // Call ConnectionStatusChanged
    SendCachedHandlerState(_rawHandlersStates, JsonTargetTypeRaw, JsonHostCreateRawHandler);
}

}}}}
