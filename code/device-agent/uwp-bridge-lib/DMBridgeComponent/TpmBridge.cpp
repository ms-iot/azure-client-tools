// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "TpmBridge.h"
#include "RpcUtilities.h"

using namespace RpcUtils;

namespace DMBridgeComponent
{
    TpmBridge::TpmBridge()
    {
        RpcUtils::RpcBind(&this->rpcBinding);
    }
    TpmBridge::~TpmBridge()
    {
        RpcUtils::RpcCloseBinding(&this->rpcBinding);
    }

    Platform::String^ TpmBridge::GetEndorsementKey()
    {
        wchar_t *ek = nullptr;
        int size = 0;
        HRESULT hr = RpcUtils::RpcNormalize(::GetEndorsementKeyRpc, this->rpcBinding, &size, &ek);
        if (FAILED(hr))
        {
            return ref new Platform::String();
        }
        return ref new Platform::String(ek);
    }

    Platform::String^ TpmBridge::GetRegistrationId()
    {
        wchar_t *registrationId = nullptr;
        int size = 0;
        HRESULT hr = RpcUtils::RpcNormalize(::GetRegistrationIdRpc, this->rpcBinding, &size, &registrationId);
        if (FAILED(hr))
        {
            return ref new Platform::String();
        }
        return ref new Platform::String(registrationId);
    }

    Platform::String^ TpmBridge::GetConnectionString(int32 slot, int64 expiry)
    {
        wchar_t *connectionString = nullptr;
        int size = 0;
        HRESULT hr = RpcUtils::RpcNormalize(::GetConnectionStringRpc, this->rpcBinding, slot, expiry, &size, &connectionString);
        if (FAILED(hr))
        {
            return ref new Platform::String();
        }
        return ref new Platform::String(connectionString);
    }
}
