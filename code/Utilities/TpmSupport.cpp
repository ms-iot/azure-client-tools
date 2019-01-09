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

std::wstring TpmSupport::Azure_EncodeBase32(std::vector<unsigned char> &pData)
{
    size_t cbData = pData.size();
    const wchar_t BASE32_VALUES[] = { L"abcdefghijklmnopqrstuvwxyz234567=" };
    const int encodingBlockSize = 5;
    size_t encodedDataLength = (((cbData + encodingBlockSize - 1) / encodingBlockSize) * 8);
    vector<wchar_t> bEncodedData(encodedDataLength, 0);

    if (pData.size() == 0)
    {
        return L"";
    }

    auto iterator{ pData.begin() };
    size_t cbBlockLength = 0;
    size_t result_len = 0;
    unsigned char pos1 = 0;
    unsigned char pos2 = 0;
    unsigned char pos3 = 0;
    unsigned char pos4 = 0;
    unsigned char pos5 = 0;
    unsigned char pos6 = 0;
    unsigned char pos7 = 0;
    unsigned char pos8 = 0;

    // Go through the source buffer sectioning off blocks of 5
    while (cbData >= 1)
    {
        pos1 = pos2 = pos3 = pos4 = pos5 = pos6 = pos7 = pos8 = 0;
        cbBlockLength = cbData > encodingBlockSize ? encodingBlockSize : cbData;
        // Fall through switch block to process the 5 (or smaller) block
        switch (cbBlockLength)
        {
        case 5:
            pos8 = (iterator[4] & 0x1f);
            pos7 = ((iterator[4] & 0xe0) >> 5);
#if 0
            // TODO: figure out proper predefined macro
            [[fallthrough]]
#else
            __fallthrough;
#endif
        case 4:
            pos7 |= ((iterator[3] & 0x03) << 3);
            pos6 = ((iterator[3] & 0x7c) >> 2);
            pos5 = ((iterator[3] & 0x80) >> 7);
#if 0
            // TODO: figure out proper predefined macro
            [[fallthrough]]
#else
            __fallthrough;
#endif
        case 3:
            pos5 |= ((iterator[2] & 0x0f) << 1);
            pos4 = ((iterator[2] & 0xf0) >> 4);
#if 0
            // TODO: figure out proper predefined macro
            [[fallthrough]]
#else
            __fallthrough;
#endif
        case 2:
            pos4 |= ((iterator[1] & 0x01) << 4);
            pos3 = ((iterator[1] & 0x3e) >> 1);
            pos2 = ((iterator[1] & 0xc0) >> 6);
#if 0
            // TODO: figure out proper predefined macro
            [[fallthrough]]
#else
            __fallthrough;
#endif
        case 1:
            pos2 |= ((iterator[0] & 0x07) << 2);
            pos1 = ((iterator[0] & 0xf8) >> 3);
            break;
        }
        // Move the iterator the block size
        iterator += cbBlockLength;
        // and decrement the src_size;
        cbData -= cbBlockLength;

        // If the src_size is not divisible by 8, base32_encode_impl shall pad the remaining places with =.
        switch (cbBlockLength)
        {
        case 1: pos3 = pos4 = 32;
        case 2: pos5 = 32;
        case 3: pos6 = pos7 = 32;
        case 4: pos8 = 32;
        case 5:
            break;
        }

        // Map the 5 bit chunks into one of the BASE32 values (a-z,2,3,4,5,6,7) values. ] */
        bEncodedData[result_len++] = BASE32_VALUES[pos1];
        bEncodedData[result_len++] = BASE32_VALUES[pos2];
        bEncodedData[result_len++] = BASE32_VALUES[pos3];
        bEncodedData[result_len++] = BASE32_VALUES[pos4];
        bEncodedData[result_len++] = BASE32_VALUES[pos5];
        bEncodedData[result_len++] = BASE32_VALUES[pos6];
        bEncodedData[result_len++] = BASE32_VALUES[pos7];
        bEncodedData[result_len++] = BASE32_VALUES[pos8];
    }

    // Sanitize the result
    size_t iResultLength = 0;
    for (auto it = bEncodedData.begin(); it != bEncodedData.end(); ++it)
    {
        if (*it == '=')
            break;
        iResultLength++;
    }

    if (cbData != iResultLength)
    {
        bEncodedData.resize(iResultLength);
    }
    return wstring(bEncodedData.begin(), bEncodedData.end());
}

HRESULT TpmSupport::GetRegistrationId(wstring &registrationId)
{
    TPM_RC result = TPM_RC_SUCCESS;
    std::vector<unsigned char> ek;
    BCRYPT_ALG_HANDLE       algHandle = INVALID_HANDLE_VALUE;
    BCRYPT_HASH_HANDLE      hHash = INVALID_HANDLE_VALUE;
    DWORD                   digestSize = 0, cbData = 0;
    std::vector<unsigned char>   digest(64, 0);

    if ((result = GetEndorsementKeyPub(ek)) != TPM_RC_SUCCESS)
    {
        //TODO: Add trace logging event.
    }
    else if ((result = BCryptOpenAlgorithmProvider(&algHandle, BCRYPT_SHA256_ALGORITHM, NULL, 0)) != 0)
    {
        //TODO: Add trace loggint event.
    }
    //calculate the length of the hash
    else if ((result = BCryptGetProperty(
        algHandle,
        BCRYPT_HASH_LENGTH,
        (PBYTE)&digestSize,
        sizeof(DWORD),
        &cbData,
        0)) != 0)
    {
        //TODO: Add trace logging event.
    }
    else if ((result = BCryptCreateHash(algHandle, &hHash, NULL, 0, NULL, 0, 0)) != 0)
    {
        //TODO: Add trace logging event.
    }
    else if ((result = BCryptHashData(hHash, &ek[0], from_sizet<ULONG>(ek.size()), 0)) != 0)
    {
        //TODO: Add trace logging event.
    }
    else if ((result = BCryptFinishHash(hHash, &digest[0], (ULONG)digestSize, 0)) != 0)
    {
        //TODO: Add trace logging event.
    }
    else
    {
        digest.resize(digestSize);
        // Encode the digest and return it as string.
        registrationId = Azure_EncodeBase32(digest);
    }

    if (algHandle != INVALID_HANDLE_VALUE)
    {
        BCryptCloseAlgorithmProvider(algHandle, 0);
    }

    if (hHash != INVALID_HANDLE_VALUE)
    {
        BCryptDestroyHash(hHash);
    }

    return HRESULT_FROM_TPM(result);
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

    return result;
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
