// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "device-agent/common/plugins/PluginJsonConstants.h"
#include "device-agent/plugin-common/PluginStub.h"
#include "PluginJsonConstants.h"
#include "UwpAppManagementHandler.h"
#include "UwpGetInstalledAppsHandler.h"
#include "UwpManageAppLifeCycleHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::UwpAppManagementPlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gUwpAppManagementHandler;
std::shared_ptr<IRawHandler> gUwpGetInstalledAppsHandler;
std::shared_ptr<IRawHandler> gUwpManageAppLifeCycleHandler;

std::shared_ptr<IRawHandler> CreateUwpAppManagementHandler()
{
    if (gUwpAppManagementHandler == nullptr)
    {
        gUwpAppManagementHandler = make_shared<UwpAppManagementHandler>();
    }
    return gUwpAppManagementHandler;
}

std::shared_ptr<IRawHandler> CreateUwpGetInsalledAppsHandler()
{
    if (gUwpGetInstalledAppsHandler == nullptr)
    {
        gUwpGetInstalledAppsHandler = make_shared<UwpGetInstalledAppsHandler>();
    }
    return gUwpGetInstalledAppsHandler;
}

std::shared_ptr<IRawHandler> CreateUwpManageAppLifeCycleHandler()
{
    if (gUwpManageAppLifeCycleHandler == nullptr)
    {
        gUwpManageAppLifeCycleHandler = make_shared<UwpManageAppLifeCycleHandler>();
    }
    return gUwpManageAppLifeCycleHandler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(UwpAppManagementHandlerId, CreateUwpAppManagementHandler);
    gPluginStub.RegisterRawHandler(UwpGetInstalledAppsHandlerId, CreateUwpGetInsalledAppsHandler);
    gPluginStub.RegisterRawHandler(UwpManageAppLifeCycleHandlerId, CreateUwpManageAppLifeCycleHandler);
    return DM_ERROR_SUCCESS;
}

#include "device-agent/plugin-common/PluginExports.h"
