// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "RpcUtilities.h"

namespace DMBridgeComponent
{
    /// <summary>
    /// Client side RPC implementation
    /// </summary>
    public ref class RebootBridge sealed
    {
    public:
        RebootBridge();
        virtual ~RebootBridge();

        void Invoke();

    private:
        RPC_BINDING_HANDLE rpcBinding = nullptr;
    };
}
