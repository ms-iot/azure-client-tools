// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\AzureDeviceManagementPluginCommon\PluginStub.h"
#include "PluginJsonConstants.h"
#include "RemoteWipeCmdHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::RemoteWipePlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gRemoteWipeCmdHandler;

std::shared_ptr<IRawHandler> CreateRemoteWipeCmdHandler()
{
    if (gRemoteWipeCmdHandler == nullptr)
    {
        gRemoteWipeCmdHandler = make_shared<RemoteWipeCmdHandler>();
    }
    return gRemoteWipeCmdHandler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(RemoteWipeCmdHandlerId, CreateRemoteWipeCmdHandler);
    return PLUGIN_ERR_SUCCESS;
}

#include "..\AzureDeviceManagementPluginCommon\PluginExports.h"
