// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\AzureDeviceManagementPluginCommon\PluginStub.h"
#include "PluginJsonConstants.h"
#include "SkeletonCmdHandler.h"
#include "SkeletonStateHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Windows::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Windows::Azure::DeviceManagement::SkeletonPlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gSkeletonCmdHandler;
std::shared_ptr<IRawHandler> gSkeletonStateHandler;

std::shared_ptr<IRawHandler> CreateSkeletonCmdHandler()
{
    if (gSkeletonCmdHandler == nullptr)
    {
        gSkeletonCmdHandler = make_shared<SkeletonCmdHandler>();
    }
    return gSkeletonCmdHandler;
}

std::shared_ptr<IRawHandler> CreateSkeletonStateHandler()
{
    if (gSkeletonStateHandler == nullptr)
    {
        gSkeletonStateHandler = make_shared<SkeletonStateHandler>();
    }
    return gSkeletonStateHandler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(SkeletonCmdHandlerId, CreateSkeletonCmdHandler);
    gPluginStub.RegisterRawHandler(SkeletonStateHandlerId, CreateSkeletonStateHandler);
    return PLUGIN_ERR_SUCCESS;
}

#include "..\AzureDeviceManagementPluginCommon\PluginExports.h"
