// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "device-agent/common/plugins/PluginJsonConstants.h"
#include "device-agent/plugin-common/PluginStub.h"
#include "PluginJsonConstants.h"
#include "TestCmdHandler.h"
#include "TestStateHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::TestPlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gTestCmdHandler;
std::shared_ptr<IRawHandler> gTestStateHandler;

std::shared_ptr<IRawHandler> CreateTestCmdHandler()
{
    if (gTestCmdHandler == nullptr)
    {
        gTestCmdHandler = make_shared<TestCmdHandler>();
    }
    return gTestCmdHandler;
}

std::shared_ptr<IRawHandler> CreateTestStateHandler()
{
    if (gTestStateHandler == nullptr)
    {
        gTestStateHandler = make_shared<TestStateHandler>();
    }
    return gTestStateHandler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(TestCmdHandlerId, CreateTestCmdHandler);
    gPluginStub.RegisterRawHandler(TestStateHandlerId, CreateTestStateHandler);
    return DM_ERROR_SUCCESS;
}

#include "device-agent/plugin-common/PluginExports.h"
