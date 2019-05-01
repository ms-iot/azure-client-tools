// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include "TpmSupport.h"
#include "DMString.h"
#include "ResultMacros.h"

#include <BCrypt.h>
#include <NCrypt.h>
#include <Wincrypt.h>
#include <CertEnroll.h>
#include <tbs.h>

#include "UrchinLib.h"
#include "UrchinPlatform.h"

#include "LimpetApi.h"

#define IOTDPSCLIENT_PARAMETERS_REGPATH             L"system\\currentcontrolset\\services\\iotdpsclient\\parameters"
#define IOTDPSCLIENT_PARAMETERS_REGNAME_TPMSLOT     L"tpm_slot"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

HRESULT TpmSupport::ImportKeyToTpm(
    UINT32 logicalDeviceNumber,
    unsigned char* pbKeyBlob,
    size_t cbKeyBlob
)
{
    TBS_RESULT result = TBS_SUCCESS;

    vector<BYTE> activationSecret(MAX_AES_KEY_BYTES);
    wstring keyName(LIMPET_STRING_SIZE, L'\0');
    wstring uriName(LIMPET_STRING_SIZE, L'\0');

    result = LimpetImportHostageKey(logicalDeviceNumber,
        pbKeyBlob,
        from_sizet<ULONG>(cbKeyBlob),
        &activationSecret[0],
        (WCHAR*)keyName.c_str(),
        (WCHAR*)uriName.c_str());

    return static_cast<HRESULT>(result);
}

HRESULT TpmSupport::GetEndorsementKeyPub(std::vector<unsigned char>& ekPub)
{
    TBS_RESULT result = TBS_SUCCESS;
    std::vector<BYTE> keyBuffer(1024);
    UINT32 cbKeyBuffer = (UINT32)keyBuffer.size();

    // Get the public key blob
    if ((result = LimpetGetTpmKey2BPub(LIMPET_TPM20_ERK_HANDLE, &keyBuffer[0], (UINT32)keyBuffer.size(), &cbKeyBuffer, NULL)) == TPM_RC_SUCCESS)
    {
        keyBuffer.resize(cbKeyBuffer);
        ekPub = keyBuffer;
    }

    return static_cast<HRESULT>(result);
}

HRESULT TpmSupport::GetStorageRootKeyPub(std::vector<unsigned char>& srkPub)
{
    TBS_RESULT result = TBS_SUCCESS;
    std::vector<BYTE> keyBuffer(1024);
    UINT32 cbKeyBuffer = (UINT32)keyBuffer.size();

    // Get the public key blob
    if ((result = LimpetGetTpmKey2BPub(LIMPET_TPM20_SRK_HANDLE, &keyBuffer[0], (UINT32)keyBuffer.size(), &cbKeyBuffer, NULL)) == TPM_RC_SUCCESS)
    {
        keyBuffer.resize(cbKeyBuffer);
        srkPub = keyBuffer;
    }

    return static_cast<HRESULT>(result);
}

HRESULT TpmSupport::InitializeTpm()
{
    return static_cast<HRESULT>(LimpetInitialize());
}

HRESULT TpmSupport::DeinitializeTpm()
{
    return static_cast<HRESULT>(LimpetDestroy());
}

HRESULT TpmSupport::IsDeviceProvisioned(_In_ UINT32 logicalDeviceNumber, _Out_ bool *provisioned)
{
    TPM_RC result = TPM_RC_SUCCESS;
    BOOL isProvisioned = false;

    try
    {
        // Read the URI from the TPM if we have any
        std::vector<BYTE> nvData(1024);
        UINT32 cbNvData = 0;
        result = LimpetReadURI(logicalDeviceNumber, &nvData[0], (UINT32)nvData.size(), &cbNvData);
        if (cbNvData != 0)
        {
            isProvisioned = true;
        }

        // LimpetReadURI raising failure when the URI is not present, so suppressing this specific error code.
        if (result == TPM_RC_FAILURE)
        {
            result = TPM_RC_SUCCESS;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }

    *provisioned = isProvisioned;
    return HRESULT_FROM_TPM(result);
}

HRESULT TpmSupport::EmptyTpmSlot(UINT32 logicalDeviceNumber)
{
    std::wstring uriName(LIMPET_STRING_SIZE, L'\0');
    std::wstring hmacKeyName(LIMPET_STRING_SIZE, L'\0');

    // Destroy the service uri stored in TPM.
    LimpetDestroyURI(logicalDeviceNumber, (WCHAR*)uriName.c_str());
    // Destroy hmac key stored in TPM.
    LimpetEvictHmacKey(logicalDeviceNumber, (WCHAR*)hmacKeyName.c_str());

    //TODO: Add additional logic to ignore specific case when Tpm has no information to clean.
    return S_OK;
}

HRESULT TpmSupport::ClearTpm()
{
    LimpetForceClearTpm();
    return S_OK;
}

HRESULT TpmSupport::GetRegistrationId(wstring &registrationId)
{
    return LimpetGetRegistrationId(registrationId);
}

HRESULT TpmSupport::GetAzureConnectionString(_In_ UINT32 logicalDeviceNumber, _In_ DWORD expiryDurationInSeconds, wstring& connectionString)
{
    HRESULT result = NO_ERROR;

    FILETIME utcTime = { 0 };
    LARGE_INTEGER expiration = { 0 };

    GetSystemTimeAsFileTime(&utcTime);
    expiration.HighPart = utcTime.dwHighDateTime;
    expiration.LowPart = utcTime.dwLowDateTime;
    expiration.QuadPart = (expiration.QuadPart / WINDOWS_TICKS_PER_SEC) - EPOCH_DIFFERENCE + expiryDurationInSeconds;

    size_t connectionStringCapacity = connectionString.capacity();
    if (connectionStringCapacity < LIMPET_STRING_SIZE)
    {
        connectionString.resize(LIMPET_STRING_SIZE);
        connectionStringCapacity = LIMPET_STRING_SIZE;
    }
    result = static_cast<HRESULT>(LimpetGenerateSASToken(logicalDeviceNumber, &expiration, const_cast<WCHAR*>(connectionString.c_str()), &connectionStringCapacity));

    return result == TPM_RC_SUCCESS ? S_OK : E_FAIL;
}

HRESULT TpmSupport::StoreIothubUri(__in UINT32 tpmSlotNumber, __in const wstring& uri, __out wstring& name)
{
    if (name.capacity() < LIMPET_STRING_SIZE)
    {
        name.resize(LIMPET_STRING_SIZE);
    }
    return
        LimpetStoreURI(
            tpmSlotNumber,
            (WCHAR*)uri.c_str(),
            (WCHAR*)name.c_str()
        );
}

HRESULT TpmSupport::GetIothubUri(__in UINT32 tpmSlotNumber, string &uriData)
{
    UINT32 result = TPM_RC_SUCCESS;

    // Read the data from the TPM if we have any
    UINT32 uriCapacity = static_cast<UINT32>(uriData.capacity());
    if (uriCapacity < LIMPET_STRING_SIZE)
    {
        uriData.resize(LIMPET_STRING_SIZE);
        uriCapacity = LIMPET_STRING_SIZE;
    }
    result = LimpetReadURI(
        tpmSlotNumber,
        (PBYTE)&uriData[0],
        (UINT32)uriCapacity,
        &uriCapacity);
    return result;
}

HRESULT TpmSupport::StoreHmacKey(__in UINT32 tpmSlotNumber, const string &key)
{
    vector<BYTE> hmacKey(SHA256_DIGEST_SIZE * 2);
    DWORD cbHmacKey = (DWORD)hmacKey.size();
    UINT32 result = TPM_RC_SUCCESS;

    wstring wModuleKey{ MultibyteToWide(key.c_str()) };

    // Decode the key
    if ((!CryptStringToBinaryW(wModuleKey.c_str(), static_cast<DWORD>(wModuleKey.size()), CRYPT_STRING_BASE64, &hmacKey[0], &cbHmacKey, nullptr, nullptr)) ||
        (cbHmacKey < SHA256_DIGEST_SIZE / 2) ||
        (cbHmacKey > SHA256_DIGEST_SIZE * 2))
    {
        result = TPM_RC_FAILURE;
    }
    else
    {
        hmacKey.resize(cbHmacKey);
        result = LimpetCreateHmacKey(tpmSlotNumber, &hmacKey[0], (UINT32)hmacKey.size(), nullptr);
    }

    return result;
}

}}}};
