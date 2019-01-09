// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// DirectRebootManagementPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "..\..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "..\..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\..\..\AzureDeviceManagementCommon\Plugins\PluginInterfaces.h"
#include "RebootInfoHandler.h"
#include "RebootCmdHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::RebootManagementPlugin;

int __stdcall PluginCreate()
{
    return PLUGIN_ERR_SUCCESS;
}

vector<HandlerInfo> __stdcall PluginGetHandlersInfo()
{
    vector<HandlerInfo> handlersInfo;

    HandlerInfo rebootInfo;
    rebootInfo.id = RebootInfoHandlerId;
    rebootInfo.type = JsonHandlerTypeRaw;

    handlersInfo.emplace_back(rebootInfo);

    HandlerInfo rebootCmd;
    rebootCmd.id = RebootCmdHandlerId;
    rebootCmd.type = JsonHandlerTypeRaw;

    handlersInfo.emplace_back(rebootCmd);

    return handlersInfo;
}

shared_ptr<IRawHandler> __stdcall PluginCreateRawHandler(const string& id)
{
    shared_ptr<IRawHandler> rawHandler;

    if (id == RebootInfoHandlerId)
    {
        rawHandler = make_shared<RebootInfoHandler>();
    }
    else if (id == RebootCmdHandlerId)
    {
        rawHandler = make_shared<RebootCmdHandler>();
    }
    else
    {
        throw DMException(PLUGIN_ERR_INVALID_RAW_HANDLER, "Unknown raw configuration handler.");
    }

    return rawHandler;
}

void __stdcall PluginDestroyRawHandler(const std::string&)
{
    // no-op
}

