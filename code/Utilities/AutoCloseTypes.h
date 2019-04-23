// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <memory>
#include <utility>
#include "AutoCloseBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    struct SIDDeleter
    {
        void operator()(PSID pSid) const { FreeSid(pSid); };
    };
    typedef std::unique_ptr<void, SIDDeleter> AutoCloseSID;

    struct ACLDeleter
    {
        void operator()(PACL pAcl) const { LocalFree(pAcl); };
    };
    typedef std::unique_ptr<ACL, ACLDeleter> AutoCloseACL;

    struct ServiceHandleDeleter
    {
        void operator()(SC_HANDLE scHandle) const { if (nullptr != scHandle) CloseServiceHandle(scHandle); };
    };
    typedef std::unique_ptr<SC_HANDLE__, ServiceHandleDeleter> AutoCloseServiceHandle;

    struct CertificateContextDeleter
    {
        void operator()(PCCERT_CONTEXT context) const { CertFreeCertificateContext(context); };
    };
    typedef std::unique_ptr<const CERT_CONTEXT, CertificateContextDeleter> AutoCloseCertificateContext;

    struct HCertStoreDeleter
    {
        void operator()(HCERTSTORE hStore) const { CertCloseStore(hStore, 0); };
    };
    typedef std::unique_ptr<void, HCertStoreDeleter> AutoCloseHCertStore;

    struct BCryptHandleDeleter
    {
        void operator()(BCRYPT_KEY_HANDLE handle) const { BCryptDestroyKey(handle); };
    };
    typedef std::unique_ptr<void, BCryptHandleDeleter> AutoCloseBcryptKeyHandle;

class AutoCloseNcryptProvHandle : public AutoCloseBase<NCRYPT_PROV_HANDLE>
{
public:
    AutoCloseNcryptProvHandle() :
        AutoCloseBase(NULL, [](NCRYPT_PROV_HANDLE h) { NCryptFreeObject(h); return TRUE; })
    {}

    AutoCloseNcryptProvHandle(NCRYPT_PROV_HANDLE&& handle) :
        AutoCloseBase(std::move(handle), [](NCRYPT_PROV_HANDLE h) { NCryptFreeObject(h); return TRUE; })
    {}

private:
    AutoCloseNcryptProvHandle(const AutoCloseNcryptProvHandle &);            // prevent copy
    AutoCloseNcryptProvHandle& operator=(const AutoCloseNcryptProvHandle&);  // prevent assignment
};

class AutoCloseNcryptKeyHandle : public AutoCloseBase<NCRYPT_KEY_HANDLE>
{
public:
    AutoCloseNcryptKeyHandle() :
        AutoCloseBase(NULL, [](NCRYPT_KEY_HANDLE h) { NCryptFreeObject(h); return TRUE; })
    {}

    AutoCloseNcryptKeyHandle(NCRYPT_KEY_HANDLE&& handle) :
        AutoCloseBase(std::move(handle), [](NCRYPT_KEY_HANDLE h) { NCryptFreeObject(h); return TRUE; })
    {}

private:
    AutoCloseNcryptKeyHandle(const AutoCloseNcryptKeyHandle &);            // prevent copy
    AutoCloseNcryptKeyHandle& operator=(const AutoCloseNcryptKeyHandle&);  // prevent assignment
};

class AutoCloseCom
{
private:
    HRESULT _hr;

public:
    AutoCloseCom()
    {
        _hr = CoInitializeEx(nullptr, 0);
    }

    ~AutoCloseCom()
    {
        if (SUCCEEDED(_hr))
        {
            CoUninitialize();
        }
    }

    const HRESULT GetInitializationStatus()
    {
        return _hr;
    }
};

}}}}
