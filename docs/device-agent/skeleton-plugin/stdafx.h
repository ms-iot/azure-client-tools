#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <memory>

#include "..\..\Utilities\Utils.h"
namespace DMUtils = Microsoft::Windows::Azure::DeviceManagement::Utils;

#include "..\..\AzureDeviceManagementCommon\DMCommon.h"
namespace DMCommon = Microsoft::Windows::Azure::DeviceManagement::Common;
