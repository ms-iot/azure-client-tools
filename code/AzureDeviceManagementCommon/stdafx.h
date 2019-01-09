// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <string>
#include <functional>
#include <vector>
#include <thread>
#include <string>
#include <queue>
#include <mutex>
#include <future>
#include <functional>
#include <condition_variable>
#include <wrl/client.h>
#include <xmllite.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "..\Utilities\Utils.h"

namespace DMUtils = Microsoft::Azure::DeviceManagement::Utils;

#include "DMInterfaces.h"

namespace DMCommon = Microsoft::Azure::DeviceManagement::Common;

