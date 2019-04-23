#pragma once

#include "Utilities/Utils.h"
namespace DMUtils = Microsoft::Azure::DeviceManagement::Utils;

#include "device-agent/common/DMCommon.h"
namespace DMCommon = Microsoft::Azure::DeviceManagement::Common;

#include "AgentBinaryProxy.h"
#include "MdmServerProxy.h"
#include "PluginStub.h"
#include "RawHandlerHostProxy.h"
#include "RawHandlersStub.h"

#ifdef _DEBUG
#pragma comment( lib, "AzureDeviceManagementPluginCommond.lib" )
#else
#pragma comment( lib, "AzureDeviceManagementPluginCommon.lib" )
#endif
