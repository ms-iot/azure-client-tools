// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\AzureDeviceManagementPluginCommon\PluginStub.h"
#include "PluginJsonConstants.h"
#include "RebootCmdHandler.h"
#include "RebootStateHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::RebootPlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gRebootCmdHandler;
std::shared_ptr<IRawHandler> gRebootStateHandler;

std::shared_ptr<IRawHandler> CreateRebootCmdHandler()
{
    if (gRebootCmdHandler == nullptr)
    {
        gRebootCmdHandler = make_shared<RebootCmdHandler>();
    }
    return gRebootCmdHandler;
}

std::shared_ptr<IRawHandler> CreateRebootStateHandler()
{
    if (gRebootStateHandler == nullptr)
    {
        gRebootStateHandler = make_shared<RebootStateHandler>();
    }
    return gRebootStateHandler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(RebootCmdHandlerId, CreateRebootCmdHandler);
    gPluginStub.RegisterRawHandler(RebootStateHandlerId, CreateRebootStateHandler);
    return PLUGIN_ERR_SUCCESS;
}

#include "..\AzureDeviceManagementPluginCommon\PluginExports.h"
