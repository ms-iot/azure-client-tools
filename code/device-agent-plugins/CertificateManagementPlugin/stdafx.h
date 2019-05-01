// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <memory>

#include "../../Utilities/Utils.h"
namespace DMUtils = Microsoft::Azure::DeviceManagement::Utils;

#include "../../device-agent/common/DMCommon.h"
namespace DMCommon = Microsoft::Azure::DeviceManagement::Common;

#include "../../device-agent/plugin-common/PluginsCommon.h"