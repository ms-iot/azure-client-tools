// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DpsBridge.h"
#include "RpcUtilities.h"

using namespace RpcUtils;

namespace DMBridgeComponent
{
    DpsBridge::DpsBridge()
    {
        RpcUtils::RpcBind(&this->rpcBinding);
    }

    DpsBridge::~DpsBridge()
    {
        RpcUtils::RpcCloseBinding(&this->rpcBinding);
    }

    void DpsBridge::SetScopeId(Platform::String ^scopeId)
    {
        RpcUtils::RpcNormalize(::SetScopeIdRpc, this->rpcBinding, scopeId->Data());
    }
}