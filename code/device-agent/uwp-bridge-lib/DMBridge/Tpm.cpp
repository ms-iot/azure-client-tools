// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Tpm.h"

#include "DMBridgeServer.h"
#include "DMException.h"

using namespace std;
using namespace Microsoft::Azure::DeviceManagement::Utils;

/* Map Generated rpc method signatures to class */
/* -------------------------------------------- */
HRESULT GetEndorsementKeyRpc(_In_ handle_t, _Outptr_ int *size, _Outptr_ wchar_t **endorsementKey)
{
    return Tpm::GetEndorsementKey(*size, *endorsementKey);
}

HRESULT GetRegistrationIdRpc(_In_ handle_t, _Outptr_ int *size, _Outptr_ wchar_t **registrationId)
{
    return Tpm::GetRegistrationId(*size, *registrationId);
}

HRESULT GetConnectionStringRpc(_In_ handle_t, _In_ INT32 slot, _In_ long long expiry, _Outptr_ int *size, _Outptr_ wchar_t **connectionString)
{
    return Tpm::GetConnectionString(slot, expiry, *size, *connectionString);
}
/* -------------------------------------------- */

static HRESULT AllocateBufferAndCopyResults(const wstring& wideString, int &size, _Outptr_ wchar_t *&buffer)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    size = 0;

    size_t bufferSize = wideString.size() + 1;
    buffer = (wchar_t*)midl_user_allocate(bufferSize * sizeof(wchar_t));
    if (buffer == NULL)
    {
        TRACELINE(LoggingLevel::Verbose, "Failed to allocate memory.");
        return E_OUTOFMEMORY;
    }

    errno_t copyErr = wcscpy_s(buffer, bufferSize, wideString.data());
    if (copyErr != 0)
    {
        TRACELINEP(LoggingLevel::Verbose, "Failed to copy buffer to out pointer. Errno: ", copyErr);
        return E_FAIL;
    }

    size = static_cast<int>(bufferSize);
    return S_OK;
}

HRESULT Tpm::GetEndorsementKey(_Outptr_ int &size, _Outptr_ wchar_t *&endorsementKey)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    try
    {
        size = 0;

        auto localDmAccess = DMBridgeServer::GetLocalDmAccess();
        wstring wideString(L"");

        localDmAccess->GetEndorsementKey(wideString);
        return AllocateBufferAndCopyResults(wideString, size, endorsementKey);
    }
    catch (const DMException& dme)
    {
        DWORD lastError = dme.Code();
        TRACELINEP(LoggingLevel::Verbose, "Failed to get registration id. Unknown exception caught. Error: ", lastError);
        // Prevent a 0'd last error from returning S_OK
        if (lastError == 0)
        {
            return E_FAIL;
        }
        return HRESULT_FROM_WIN32(lastError);
    }
}

HRESULT Tpm::GetRegistrationId(_Outptr_ int &size, _Outptr_ wchar_t *&registrationId)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    try
    {
        size = 0;

        auto localDmAccess = DMBridgeServer::GetLocalDmAccess();
        wstring wideString(L"");

        localDmAccess->GetRegistrationId(wideString);
        return AllocateBufferAndCopyResults(wideString, size, registrationId);
    }
    catch (const DMException& dme)
    {
        DWORD lastError = dme.Code();
        TRACELINEP(LoggingLevel::Verbose, "Failed to get registration id. Unknown exception caught. Error: ", lastError);
        // Prevent a 0'd last error from returning S_OK
        if (lastError == 0)
        {
            return E_FAIL;
        }
        return HRESULT_FROM_WIN32(lastError);
    }
}

HRESULT Tpm::GetConnectionString(_In_ int slot, _In_ long long expiry, int &size, _Outptr_ wchar_t *&connectionString)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    try
    {
        size = 0;

        auto localDmAccess = DMBridgeServer::GetLocalDmAccess();
        wstring wideString(L"");

        localDmAccess->GetConnetionString(slot, expiry, wideString);
        return AllocateBufferAndCopyResults(wideString, size, connectionString);
    }
    catch (const DMException& dme)
    {
        DWORD lastError = dme.Code();
        TRACELINEP(LoggingLevel::Verbose, "Failed to get connection string. Unknown exception caught. Error: ", lastError);
        // Prevent a 0'd last error from returning S_OK
        if (lastError == 0)
        {
            return E_FAIL;
        }
        return HRESULT_FROM_WIN32(lastError);
    }
}

