// $safeprojectname$Plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "device-agent\common\plugins\PluginConstants.h"
#include "device-agent\common\plugins\PluginJsonConstants.h"
#include "device-agent\plugin-common\PluginsCommon.h"
#include "$safeprojectname$StateHandler.h"
#include "$safeprojectname$Cmd1Handler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> g$safeprojectname$StateHandler;
std::shared_ptr<IRawHandler> g$safeprojectname$Cmd1Handler;

std::shared_ptr<IRawHandler> Create$safeprojectname$StateHandler()
{
    if (g$safeprojectname$StateHandler == nullptr)
    {
        g$safeprojectname$StateHandler = make_shared<$safeprojectname$StateHandler>();
    }
    return g$safeprojectname$StateHandler;
}

std::shared_ptr<IRawHandler> Create$safeprojectname$Cmd1Handler()
{
    if (g$safeprojectname$Cmd1Handler == nullptr)
    {
        g$safeprojectname$Cmd1Handler = make_shared<$safeprojectname$StateHandler>();
    }
    return g$safeprojectname$Cmd1Handler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler($safeprojectname$StateHandlerId, Create$safeprojectname$StateHandler);
    gPluginStub.RegisterRawHandler($safeprojectname$Cmd1HandlerId, Create$safeprojectname$Cmd1Handler);
    return DM_ERROR_SUCCESS;
}

#include "device-agent\plugin-common\PluginExports.h"