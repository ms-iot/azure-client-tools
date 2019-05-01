// TemperatureSensorPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "PluginCommon\PluginsCommon.h"
#include "TemperatureSensorStateHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gPluginA;

std::shared_ptr<IRawHandler> CreateTemperatureSensorHandler()
{
    if (gPluginA == nullptr)
    {
        gPluginA = make_shared<TemperatureSensorStateHandler>();
    }
    return gPluginA;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(TemperatureSensorStateHandlerId, CreateTemperatureSensorHandler);
    return PLUGIN_ERR_SUCCESS;
}

#include "PluginCommon\PluginExports.h"
