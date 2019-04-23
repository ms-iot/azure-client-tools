// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Reboot.h"

#include "DMBridgeServer.h"
#include "DMException.h"

using namespace std;
using namespace Microsoft::Azure::DeviceManagement::Utils;

/* Map Generated rpc method signatures to class */
/* -------------------------------------------- */
HRESULT InvokeRpc(_In_ handle_t)
{
    return Reboot::Invoke();
}
/* -------------------------------------------- */

HRESULT Reboot::Invoke()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    try
    {
        auto localDmAccess = DMBridgeServer::GetLocalDmAccess();
        localDmAccess->InvokeReboot();
        return S_OK;
    }
    catch (const DMException& dme)
    {
        DWORD lastError = dme.Code();
        TRACELINEP(LoggingLevel::Verbose, "Failed to reboot. Unknown exception caught. Error: ", lastError);
        // Prevent a 0'd last error from returning S_OK
        if (lastError == 0)
        {
            return E_FAIL;
        }
        return HRESULT_FROM_WIN32(lastError);
    }
}
