// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\AzureDeviceManagementPluginCommon\PluginStub.h"
#include "PluginJsonConstants.h"
#include "WindowsTelemetryStateHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::WindowsTelemetryPlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gWindowsTelemetryStateHandler;

std::shared_ptr<IRawHandler> CreateWindowsTelemetryStateHandler()
{
    if (gWindowsTelemetryStateHandler == nullptr)
    {
        gWindowsTelemetryStateHandler = make_shared<WindowsTelemetryStateHandler>();
    }
    return gWindowsTelemetryStateHandler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(WindowsTelemetryStateHandlerId, CreateWindowsTelemetryStateHandler);
    return PLUGIN_ERR_SUCCESS;
}

#include "..\AzureDeviceManagementPluginCommon\PluginExports.h"
