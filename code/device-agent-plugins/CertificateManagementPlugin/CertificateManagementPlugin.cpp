// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// CertificateManagementPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "device-agent/common/plugins/PluginJsonConstants.h"
#include "device-agent/plugin-common/PluginStub.h"
#include "PluginJsonConstants.h"
#include "CertificateManagementHandler.h"
#include "CertificateDetailedInfoHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::CertificateManagementPlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gCertificateManagement;
std::shared_ptr<IRawHandler> gCertificateInfo;

std::shared_ptr<IRawHandler> CreateCertificateManagementHandler()
{
    if (gCertificateManagement == nullptr)
    {
        gCertificateManagement = make_shared<CertificateManagementHandler>();
    }
    return gCertificateManagement;
}

std::shared_ptr<IRawHandler> CreateCertificateInfoHandler()
{
    if (gCertificateInfo == nullptr)
    {
        gCertificateInfo = make_shared<CertificateDetailedInfoHandler>();
    }
    return gCertificateInfo;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(CertificateManagementHandlerId, CreateCertificateManagementHandler);
    gPluginStub.RegisterRawHandler(CertificateDetailedInfoHandlerId, CreateCertificateInfoHandler);
    return DM_ERROR_SUCCESS;
}

#include "device-agent/plugin-common/PluginExports.h"
