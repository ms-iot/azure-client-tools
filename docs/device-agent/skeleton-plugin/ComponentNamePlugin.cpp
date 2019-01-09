// ComponentNamePlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\AzureDeviceManagementPluginCommon\PluginStub.h"
#include "PluginJsonConstants.h"
#include "ComponentNameHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Windows::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Windows::Azure::DeviceManagement::ComponentNamePlugin;

// ToDo: Need plan for the life time management of this object.
PluginStub gPluginStub;
std::shared_ptr<IConfigurationHandler> gComponentName;

std::shared_ptr<IConfigurationHandler> CreateComponentNameHandler()
{
    if (gComponentName == nullptr)
    {
        gComponentName = make_shared<ComponentNameHandler>();
    }
    return gComponentName;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(ComponentNameHandlerId, CreateComponentNameHandler);
    return PLUGIN_ERR_SUCCESS;
}

#include "..\AzureDeviceManagementPluginCommon\PluginExports.h"
