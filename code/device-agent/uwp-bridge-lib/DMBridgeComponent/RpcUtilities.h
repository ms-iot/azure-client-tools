// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"
#include <Rpc.h>

namespace RpcUtils
{
    template<typename Function, typename handle_t, typename... Args>
    HRESULT RpcNormalize(Function f, handle_t handle, Args... args) {
        RpcTryExcept
        {
            return f(handle, args...);
        }
            RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
        {
            return HRESULT_FROM_WIN32(RpcExceptionCode());
        }
        RpcEndExcept
    };

    RPC_STATUS RpcBind(RPC_BINDING_HANDLE* rpcHandle);
    void RpcCloseBinding(RPC_BINDING_HANDLE* rpcHandle);
}