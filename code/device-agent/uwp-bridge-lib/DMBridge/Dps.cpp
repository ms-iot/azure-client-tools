// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Dps.h"

#include "DMBridgeServer.h"
#include "DMException.h"

using namespace std;
using namespace Microsoft::Azure::DeviceManagement::Utils;

// Define the private mutex
mutex Dps::_setScopeIdMutex;

/* Map Generated rpc method signatures to class */
/* -------------------------------------------- */
HRESULT SetScopeIdRpc(_In_ handle_t, _In_ const wchar_t *scopeId)
{
    return Dps::SetScopeId(scopeId);
}
/* -------------------------------------------- */

HRESULT Dps::SetScopeId(_In_ const wstring& scopeId)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    try
    {
        auto localDmAccess = DMBridgeServer::GetLocalDmAccess();
        localDmAccess->SetScopeId(scopeId);
        return S_OK;
    }
    catch (const DMException& dme)
    {
        DWORD lastError = dme.Code();
        TRACELINEP(LoggingLevel::Verbose, "Failed to set scope id. Unknown exception caught. Error: ", lastError);
        // Prevent a 0'd last error from returning S_OK
        if (lastError == 0)
        {
            return E_FAIL;
        }
        return HRESULT_FROM_WIN32(lastError);
    }
}
