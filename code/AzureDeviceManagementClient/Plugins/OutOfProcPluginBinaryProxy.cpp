// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "OutOfProcPluginBinaryProxy.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginHelpers.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\CrossBinaryRequest.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "AgentStub.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginNamedPipeTransport.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

OutOfProcPluginBinaryProxy::OutOfProcPluginBinaryProxy(const std::string& pluginFullPath, long keepAliveTime)
{
    std::wstring widePluginPath = Utils::MultibyteToWide(pluginFullPath.c_str());
    _transport = make_shared<DMCommon::PluginNamedPipeTransport>(true, widePluginPath, keepAliveTime, L"", L"");
}

void OutOfProcPluginBinaryProxy::Load()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Send create request
    std::shared_ptr<Message> createRequest = make_shared<Message>();
    createRequest->messageType = Request;
    createRequest->callType = PluginCreateCall;
    createRequest->errorCode = PLUGIN_ERR_SUCCESS;
    std::memcpy(createRequest->data, PLUGIN_CREATE_REQUEST, strlen(PLUGIN_CREATE_REQUEST) + 1);

    std::shared_ptr<Message> createResponse = _transport->SendAndGetResponse(createRequest);
    
    if (createResponse->errorCode != PLUGIN_ERR_SUCCESS)
    {
        throw DMException(createResponse->errorCode, "PluginCreate returned error.");
    }

    // 4. Set reverse invoke pointers
    _transport->SetClientInterface(AgentStub::ReverseInvoke, AgentStub::ReverseDeleteBuffer);
}

void OutOfProcPluginBinaryProxy::Unload()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Close the transport
    _transport->CloseTransport();
}

Json::Value OutOfProcPluginBinaryProxy::Invoke(
    const std::string& targetType,
    const std::string& targetId,
    const std::string& targetMethod,
    const Json::Value& targetParameters) const
{
    if (_transport == nullptr)
    {
        throw DMException(PLUGIN_TRANSPORT_NOT_INITIALIZED, "Transport is not initialized");
    }

    // If this is a CreateRawHandler call, save the handler in the transport, this is later used to recreate the 
    // raw handlers if the process host has exited 
    if (targetType.compare(JsonTargetTypeHost) == 0 && targetMethod.compare(JsonHostCreateRawHandler) == 0)
    {
        _transport->SaveRawHandler(targetParameters[JsonHostCreateRawHandlerId].asCString());
    }

    string request = CrossBinaryRequest::BuildRequest(targetType, targetId, targetMethod, targetParameters);

    // Send invoke request
    std::shared_ptr<Message> invokeRequest = make_shared<Message>();
    invokeRequest->messageType = Request;
    invokeRequest->callType = PluginInvokeCall;
    invokeRequest->errorCode = PLUGIN_ERR_SUCCESS;
    strcpy_s(invokeRequest->data, request.c_str());

    std::shared_ptr<Message> invokeResponse = _transport->SendAndGetResponse(invokeRequest);
    if (invokeResponse->errorCode != PLUGIN_ERR_SUCCESS)
    {
        throw DMException(invokeResponse->errorCode, "PluginInvoke returned error.");
    }

    if (invokeResponse->data == nullptr)
    {
        throw DMException(PLUGIN_ERR_INVALID_OUTPUT_BUFFER, "PluginInvoke returned invalid buffer.");
    }

    // Copy to local heap.
    string responseString = invokeResponse->data;

    // Translates failures to exceptions.
    return CrossBinaryResponse::GetPayloadFromJsonString(responseString);
}
}}}}
