// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DMException.h"
#include "DMString.h"
#include "Logger.h"
#include "Registry.h"
#include "AutoCloseBase.h"
#include "AutoCloseHandle.h"
#include "DMThread.h"
#include "DMProcess.h"
#include "DateTime.h"
#include "ResultMacros.h"
#include "TaskQueue.h"
#include "TpmSupport.h"
#include "json/json.h"
#include "JsonUtils.h"
#include "FileSystem.h"
#include "File.h"
#include "Guid.h"
#include "Shell.h"
#include "Impersonator.h"
#include "Version.h"
#include "DependencySort.h"
#ifdef _DEBUG
#pragma comment( lib, "AzureDeviceManagementUtilitiesd.lib" )
#else
#pragma comment( lib, "AzureDeviceManagementUtilities.lib" )
#endif