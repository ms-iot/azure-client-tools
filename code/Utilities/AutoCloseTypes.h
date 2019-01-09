// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <utility>
#include "AutoCloseBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

class AutoCloseSID : public AutoCloseBase<PSID>
{
public:
    AutoCloseSID() :
        AutoCloseBase(NULL, [](PSID h) { CloseHandle(h); return TRUE; })
    {}

    AutoCloseSID(PSID&& handle) :
        AutoCloseBase(std::move(handle), [](PSID h) { FreeSid(h); return TRUE; })
    {}

private:
    AutoCloseSID(const AutoCloseSID &);            // prevent copy
    AutoCloseSID& operator=(const AutoCloseSID&);  // prevent assignment
};

class AutoCloseACL : public AutoCloseBase<PACL>
{
public:
    AutoCloseACL() :
        AutoCloseBase(NULL, [](PSID h) { CloseHandle(h); return TRUE; })
    {}

    AutoCloseACL(PACL&& handle) :
        AutoCloseBase(std::move(handle), [](PACL h) { LocalFree(h); return TRUE; })
    {}

private:
    AutoCloseACL(const AutoCloseACL &);            // prevent copy
    AutoCloseACL& operator=(const AutoCloseACL&);  // prevent assignment
};

class AutoCloseServiceHandle : public AutoCloseBase<SC_HANDLE>
{
public:
    AutoCloseServiceHandle() :
        AutoCloseBase(NULL, [](SC_HANDLE h) { CloseServiceHandle(h); return TRUE; })
    {}

    AutoCloseServiceHandle(SC_HANDLE&& handle) :
        AutoCloseBase(std::move(handle), [](SC_HANDLE h) { CloseServiceHandle(h); return TRUE; })
    {}

private:
    AutoCloseServiceHandle(const AutoCloseServiceHandle&);             // prevent copy
    AutoCloseServiceHandle& operator=(const AutoCloseServiceHandle&);  // prevent assignment
};

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

class AutoCloseCertificateContext : public AutoCloseBase<PCCERT_CONTEXT>
{
public:
    AutoCloseCertificateContext() :
        AutoCloseBase(NULL, [](PCCERT_CONTEXT h) { CertFreeCertificateContext(h); return TRUE; })
    {}

    AutoCloseCertificateContext(PCCERT_CONTEXT&& handle) :
        AutoCloseBase(std::move(handle), [](PCCERT_CONTEXT h) { CertFreeCertificateContext(h); return TRUE; })
    {}

    AutoCloseCertificateContext& operator=(PCCERT_CONTEXT context)
    {
        Close();

        if (context != nullptr)
        {
            SetHandle(context);
        }
        return *this;
    }
private:
    AutoCloseCertificateContext(const AutoCloseCertificateContext &);            // prevent copy
    
};

class AutoCloseHCertStore : public AutoCloseBase<HCERTSTORE>
{
public:
    AutoCloseHCertStore() :
        AutoCloseBase(NULL, [](HCERTSTORE h) { CertCloseStore(h, 0); return TRUE; })
    {}

    AutoCloseHCertStore(HCERTSTORE&& handle) :
        AutoCloseBase(std::move(handle), [](HCERTSTORE h) { CertCloseStore(h, 0); return TRUE; })
    {}

private:
    AutoCloseHCertStore(const AutoCloseHCertStore &);            // prevent copy
    AutoCloseHCertStore& operator=(const AutoCloseHCertStore&);  // prevent assignment
};

class AutoCloseBcryptKeyHandle : public AutoCloseBase<BCRYPT_KEY_HANDLE>
{
public:
    AutoCloseBcryptKeyHandle() :
        AutoCloseBase(NULL, [](BCRYPT_KEY_HANDLE h) { BCryptDestroyKey(h); return TRUE; })
    {}

    AutoCloseBcryptKeyHandle(BCRYPT_KEY_HANDLE&& handle) :
        AutoCloseBase(std::move(handle), [](BCRYPT_KEY_HANDLE h) { BCryptDestroyKey(h); return TRUE; })
    {}

private:
    AutoCloseBcryptKeyHandle(const AutoCloseBcryptKeyHandle &);            // prevent copy
    AutoCloseBcryptKeyHandle& operator=(const AutoCloseBcryptKeyHandle&);  // prevent assignment
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
        CoUninitialize();
    }

    const HRESULT GetInitializationStatus()
    {
        return _hr;
    }
};

}}}}
