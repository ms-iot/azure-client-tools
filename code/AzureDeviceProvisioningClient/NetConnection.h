// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <chrono>
#include <netlistmgr.h>
#include <wrl.h>
#include <intrin.h>
#include "..\Utilities\AutoCloseHandle.h"
#include "..\Utilities\ResultMacros.h"

constexpr DWORD DEFAULT_NETWORK_CONNECTION_TIMEOUT{30000};

class CNetConnectionState :
    public ::Microsoft::WRL::RuntimeClass<
    ::Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>,
        ::Microsoft::WRL::FtmBase,
        INetworkListManagerEvents
    > {
    ::Microsoft::WRL::ComPtr<INetworkListManager> m_mgr;
    Microsoft::Azure::DeviceManagement::Utils::AutoCloseHandle m_NetworkConnected;
    DWORD m_cookie;
    IConnectionPoint* m_cp;

public:
    CNetConnectionState() {
        m_NetworkConnected.SetHandle(CreateEvent(nullptr, TRUE /*manual reset*/, FALSE /*not set*/, nullptr));
        if (m_NetworkConnected.Get() == NULL)
        {
            // LOGFATALERRORP("CreateEvent() failed. Error: %x", GetLastError());
            return;
        }
    }
    HRESULT RuntimeClassInitialize();

    STDMETHOD(ConnectivityChanged)(_In_ NLM_CONNECTIVITY NewConnectivity);
    HRESULT WaitForConnection();
    ~CNetConnectionState() {
    };
};





