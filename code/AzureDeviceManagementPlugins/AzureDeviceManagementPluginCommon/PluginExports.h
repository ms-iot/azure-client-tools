// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// ToDo: use common paths
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\CrossBinaryResponse.h"

using namespace std;
using namespace DMCommon;
using namespace DMUtils;

int __stdcall PluginInvoke(const char* jsonInputString, char** jsonOutputString)
{
    if (jsonInputString == NULL || jsonOutputString == NULL)
    {
        return PLUGIN_ERR_INVALID_ARG;
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

    return PLUGIN_ERR_SUCCESS;
}

int __stdcall PluginDeleteBuffer(const char* buffer)
{
    delete[] buffer;
    return PLUGIN_ERR_SUCCESS;
}

int __stdcall PluginSetReverseInvoke(PluginReverseInvokePtr ReverseInvoke)
{
    gPluginStub.SetReverseInvoke(ReverseInvoke);
    return PLUGIN_ERR_SUCCESS;
}

int __stdcall PluginSetReverseDeleteBuffer(PluginReverseDeleteBufferPtr ReverseDeleteBuffer)
{
    gPluginStub.SetReverseDeleteBuffer(ReverseDeleteBuffer);
    return PLUGIN_ERR_SUCCESS;
}

