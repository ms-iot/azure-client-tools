// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "RpcUtilities.h"
#include "RpcConstants.h"

namespace RpcUtils
{
    RPC_STATUS RpcBind(RPC_BINDING_HANDLE* rpcHandle)
    {
        RPC_STATUS status = RPC_S_OK;
        RPC_WSTR stringBinding = nullptr;

        status = RpcStringBindingCompose(
            NULL,
            (RPC_WSTR)RPC_PROTOCOL_SEQUENCE,
            NULL,
            (RPC_WSTR)RPC_ENDPOINT,
            NULL,
            &stringBinding);
        if (status != RPC_S_OK)
        {
            return status;
        }

        status = RpcBindingFromStringBinding(stringBinding, rpcHandle);
        if (status != RPC_S_OK)
        {
            return status;
        }

        RpcStringFree(&stringBinding);
        return status;
    }

    void RpcCloseBinding(RPC_BINDING_HANDLE* rpcHandle)
    {
        if (rpcHandle == nullptr)
            return;

        RpcBindingFree(rpcHandle);
        rpcHandle = nullptr;
    }
}

/******************************************************/
/*         MIDL allocate and free                     */
/******************************************************/

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}