// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// DeviceInfoPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "device-agent/common/plugins/PluginJsonConstants.h"
#include "device-agent/plugin-common/PluginStub.h"
#include "PluginJsonConstants.h"
#include "DeviceInfoHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::DeviceInfoPlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gDeviceInfo;

std::shared_ptr<IRawHandler> CreateDeviceInfoHandler()
{
    if (gDeviceInfo == nullptr)
    {
        gDeviceInfo = make_shared<DeviceInfoHandler>();
    }
    return gDeviceInfo;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(DeviceInfoHandlerId, CreateDeviceInfoHandler);
    return DM_ERROR_SUCCESS;
}

#include "device-agent/plugin-common/PluginExports.h"
