// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "device-agent/common/plugins/PluginJsonConstants.h"
#include "device-agent/plugin-common/PluginStub.h"
#include "PluginJsonConstants.h"
#include "TimeStateHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::TimePlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gTimeStateHandler;

std::shared_ptr<IRawHandler> CreateTimeStateHandler()
{
    if (gTimeStateHandler == nullptr)
    {
        gTimeStateHandler = make_shared<TimeStateHandler>();
    }
    return gTimeStateHandler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(TimeStateHandlerId, CreateTimeStateHandler);
    return DM_ERROR_SUCCESS;
}

#include "device-agent/plugin-common/PluginExports.h"
