// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "RpcUtilities.h"

namespace DMBridgeComponent
{
    /// <summary>
    /// Client side RPC implementation
    /// </summary>
    public ref class TpmBridge sealed
    {
    public:
        TpmBridge();
        virtual ~TpmBridge();

        Platform::String^ GetEndorsementKey();
        Platform::String^ GetRegistrationId();
        Platform::String^ GetConnectionString(int32 slot, int64 expiry);

    private:
        RPC_BINDING_HANDLE rpcBinding = nullptr;
    };
}
