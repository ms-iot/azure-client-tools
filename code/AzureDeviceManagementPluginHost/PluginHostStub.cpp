// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginHostStub.h"
#include "..\AzureDeviceManagementCommon\Plugins\PluginInterfaces.h"

using namespace std;
using namespace DMCommon;
using namespace DMUtils;
using namespace Microsoft::Azure::DeviceManagement::Client;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client { 

    std::shared_ptr<DMCommon::IPluginTransport> PluginHostStub::_transport;

    void PluginHostStub::SetPluginTransport(std::shared_ptr<DMCommon::IPluginTransport> transport)
    {
        _transport = transport;
    }

    int __stdcall PluginHostStub::ReverseInvoke(const char* jsonInputString, char** jsonOutputString)
    {
        if (jsonInputString == NULL || jsonOutputString == NULL)
        {
            return PLUGIN_ERR_INVALID_ARG;
        }

        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Send create request
        std::shared_ptr<Message> reverseInvokeRequest = std::make_shared<Message>();
        reverseInvokeRequest->messageType = Request;
        reverseInvokeRequest->callType = ReverseInvokeCall;
        reverseInvokeRequest->errorCode = PLUGIN_ERR_SUCCESS;
        std::memcpy(reverseInvokeRequest->data, jsonInputString, strlen(jsonInputString) + 1);

        std::shared_ptr<Message> response = _transport->SendAndGetResponse(reverseInvokeRequest);

        *jsonOutputString = new char[strlen(response->data) + 1];
        std::memcpy(*jsonOutputString, response->data, strlen(response->data) + 1);

        return PLUGIN_ERR_SUCCESS;
    }

    int __stdcall PluginHostStub::ReverseDeleteBuffer(const char* buffer)
    {
        // this call is no-op
        buffer;
        return PLUGIN_ERR_SUCCESS;
    }

}}}}
