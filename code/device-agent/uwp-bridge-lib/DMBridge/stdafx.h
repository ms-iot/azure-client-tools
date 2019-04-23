// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "targetver.h"

#include <fstream>
#include <set>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <regex>
#include <mutex>
#include <atlbase.h>
#include <sddl.h>
#include <securitybaseapi.h>
#include <AclAPI.h>
#include <comutil.h>
#include <Windows.h>

// There is an error in the system header files that incorrectly
// places RpcStringBindingCompose in the app partition.
// Work around it by changing the WINAPI_FAMILY to desktop temporarily.
#pragma push_macro("WINAPI_FAMILY")
#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#include "DMBridgeInterface_h.h"
#pragma pop_macro("WINAPI_FAMILY")