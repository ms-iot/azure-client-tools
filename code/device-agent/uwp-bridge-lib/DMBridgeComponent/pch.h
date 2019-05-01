// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define NOMINMAX

// There is an error in the system header files that incorrectly
// places RpcStringBindingCompose in the app partition.
// Work around it by changing the WINAPI_FAMILY to desktop temporarily.
#pragma push_macro("WINAPI_FAMILY")
#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#include "DMBridgeInterface_h.h"
#pragma pop_macro("WINAPI_FAMILY")
