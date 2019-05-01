// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "RebootBridge.h"
#include "RpcUtilities.h"

using namespace RpcUtils;

namespace DMBridgeComponent
{
    RebootBridge::RebootBridge()
    {
        RpcUtils::RpcBind(&this->rpcBinding);
    }

    RebootBridge::~RebootBridge()
    {
        RpcUtils::RpcCloseBinding(&this->rpcBinding);
    }

    void RebootBridge::Invoke()
    {
        RpcUtils::RpcNormalize(::InvokeRpc, this->rpcBinding);
    }
}
