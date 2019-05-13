// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "../../common/DMInterfaces.h"

class DMBridgeServer
{
public:
    static void Setup(std::shared_ptr<Microsoft::Azure::DeviceManagement::Common::ILocalDMClient> localDmAccess);
    static void Listen(void);
    static void StopListening(void);

    static std::shared_ptr<Microsoft::Azure::DeviceManagement::Common::ILocalDMClient> GetLocalDmAccess();

private:
    static SECURITY_DESCRIPTOR GenerateSecurityDescriptor(const WCHAR* customCapability);
    static void RegisterInterfaces(SECURITY_DESCRIPTOR* securityDescriptor, const std::vector<RPC_IF_HANDLE>& interfaces);
    static void UnregisterInterfaces(const std::vector<RPC_IF_HANDLE>& interfaces);

    static std::shared_ptr<Microsoft::Azure::DeviceManagement::Common::ILocalDMClient> _localDmAccess;
};