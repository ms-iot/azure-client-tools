// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "NetConnection.h"

using namespace std;
using namespace std::chrono;
using namespace Microsoft::WRL;
using namespace Microsoft::Azure::DeviceManagement::Utils;

HRESULT CNetConnectionState::WaitForConnection() {
    HRESULT hr = S_OK;
    DWORD dwRc = 0;
    if (WAIT_TIMEOUT != (dwRc = WaitForSingleObject(m_NetworkConnected.GetReference(), DEFAULT_NETWORK_CONNECTION_TIMEOUT)))
    {
        if (WAIT_OBJECT_0 != dwRc)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    else
    {
        hr = HRESULT_FROM_WIN32(ERROR_NO_NETWORK);
    }
    return hr;
}

STDMETHODIMP CNetConnectionState::ConnectivityChanged(_In_ NLM_CONNECTIVITY NewConnectivity) {
    if (NewConnectivity & (NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET)) {
        (VOID)SetEvent(m_NetworkConnected.GetReference());
        m_cp->Unadvise(m_cookie); // explicitly break circ ref
    }
    return S_OK;
}

HRESULT CNetConnectionState::RuntimeClassInitialize() {
    FAIL_FAST_IF_FAILED(::CoCreateInstance(CLSID_NetworkListManager,
                                           nullptr,
                                           CLSCTX_ALL,
                                           IID_PPV_ARGS(&m_mgr)), __FUNCTION__);
    NLM_CONNECTIVITY status;
    ComPtr<IConnectionPointContainer> cont;
    FAIL_FAST_IF_FAILED(m_mgr.As(&cont), __FUNCTION__);
    ComPtr<IConnectionPoint> pt;
    FAIL_FAST_IF_FAILED(cont->FindConnectionPoint(__uuidof(INetworkListManagerEvents), &pt), __FUNCTION__);
    DWORD cookie{0};
    FAIL_FAST_IF_FAILED(pt->Advise(static_cast<INetworkListManagerEvents*>(this), &cookie), __FUNCTION__); // we've now created a circular ref that must be manually broken
    m_cp = pt.Get();
    m_cookie = cookie;
    m_mgr->GetConnectivity(&status);
    FAIL_FAST_IF_FAILED(ConnectivityChanged(status), __FUNCTION__);
    return S_OK;
}
