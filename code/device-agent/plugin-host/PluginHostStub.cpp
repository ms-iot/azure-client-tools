// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginHostStub.h"
#include "../common/plugins/PluginInterfaces.h"

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
            return DM_ERROR_INVALID_ARGUMENT;
        }

        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Send create request
        std::shared_ptr<Message> reverseInvokeRequest = std::make_shared<Message>();
        reverseInvokeRequest->messageType = Request;
        reverseInvokeRequest->callType = ReverseInvokeCall;
        reverseInvokeRequest->errorCode = DM_ERROR_SUCCESS;
        reverseInvokeRequest->SetData(jsonInputString, strlen(jsonInputString) + 1);

        shared_ptr<Message> response = _transport->SendAndGetResponse(reverseInvokeRequest);
        size_t responseLength = strlen(response->Payload()) + 1;
        *jsonOutputString = new char[responseLength];
        std::memcpy(*jsonOutputString, response->Payload(), responseLength);

        return DM_ERROR_SUCCESS;
    }

    int __stdcall PluginHostStub::ReverseDeleteBuffer(const char* buffer)
    {
        // this call is no-op
        buffer;
        return DM_ERROR_SUCCESS;
    }

}}}}
