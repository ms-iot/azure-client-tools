// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// ToDo: use common paths
#include "device-agent/common/plugins/PluginJsonConstants.h"
#include "device-agent/common/plugins/CrossBinaryResponse.h"

using namespace std;
using namespace DMCommon;
using namespace DMUtils;

int __stdcall PluginInvoke(const char* jsonInputString, char** jsonOutputString)
{
    if (jsonInputString == NULL || jsonOutputString == NULL)
    {
        return DM_ERROR_INVALID_ARGUMENT;
    }

    //
    // Invoke() noexcept
    //
    // Inputs
    //   jsonInputString : json string of type CrossBinaryRequest.
    //
    // Returns:
    //   json string of type CrossBinaryResponse.
    //
    string resultString = gPluginStub.Invoke(jsonInputString);

    *jsonOutputString = new char[resultString.size() + 1];
    memcpy(*jsonOutputString, resultString.c_str(), resultString.size() + 1);

    return DM_ERROR_SUCCESS;
}

int __stdcall PluginDeleteBuffer(const char* buffer)
{
    delete[] buffer;
    return DM_ERROR_SUCCESS;
}

int __stdcall PluginSetReverseInvoke(PluginReverseInvokePtr ReverseInvoke)
{
    gPluginStub.SetReverseInvoke(ReverseInvoke);
    return DM_ERROR_SUCCESS;
}

int __stdcall PluginSetReverseDeleteBuffer(PluginReverseDeleteBufferPtr ReverseDeleteBuffer)
{
    gPluginStub.SetReverseDeleteBuffer(ReverseDeleteBuffer);
    return DM_ERROR_SUCCESS;
}

