// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Utilities/Utils.h"
namespace DMUtils = Microsoft::Azure::DeviceManagement::Utils;

#include "DMJsonConstants.h"
#include "DMInterfaces.h"
#include "JsonHelpers.h"
#include "MetaData.h"
#include "ReportedSchema.h"
#include "ReportedError.h"
#include "ReportedErrorList.h"
#include "ReportedSummary.h"
#include "Operation.h"
#include "HandlerBase.h"

#include "CSPs\MdmProxy.h"

#include "Plugins\PluginConstants.h"
#include "Plugins\PluginJsonConstants.h"
#include "Plugins\PluginHelpers.h"
#include "Plugins\CrossBinaryRequest.h"
#include "Plugins\CrossBinaryResponse.h"

#ifdef _DEBUG
#pragma comment( lib, "AzureDeviceManagementCommond.lib" )
#else
#pragma comment( lib, "AzureDeviceManagementCommon.lib" )
#endif
