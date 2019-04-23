// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <safeint.h>

// Azure HMAC encoding Table
#define DAT(__str) {(sizeof(__str) / sizeof(wchar_t)) - 1, __str}

const std::wstring REG_KEY_SUBPATH_DMCLIENT_NTSERVICE = L"SYSTEM\\CurrentControlSet\\Services\\AzureDeviceManagementClient\\Parameters";
const std::wstring REG_KEY_NAME_USEREGISTRY = L"useregistry";
const std::wstring REG_KEY_NAME_LAZYLOADINGENABLED = L"lazyloadingenabled";
const std::wstring REG_KEY_NAME_HMACPUBLICKEYNAME = L"hmacpublic";
const std::wstring REG_KEY_NAME_HMACPRIVATEKEYNAME = L"hmacprivate";
const std::wstring REG_KEY_NAME_HMACOBJECTNAME = L"hmacname";
const std::wstring REG_KEY_NAME_HOSTAGEBLOB = L"hostageblob";
const std::wstring REG_KEY_NAME_IOTHUBSERVICEURI = L"uri";
const std::wstring FIXED_URI_OBJECT_NAME_FOR_REGISTRY{ L"NoObjectName" };
const std::wstring FIXED_HMAC_OBJECT_NAME_FOR_REGISTRY{ L"NoObjectName" };

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace msl::utilities;

static const struct {
    size_t numberOfChars;
    const WCHAR* encoding;
} urlEncoding[] = {
    DAT(L"\0"),
    DAT(L"%01"),
    DAT(L"%02"),
    DAT(L"%03"),
    DAT(L"%04"),
    DAT(L"%05"),
    DAT(L"%06"),
    DAT(L"%07"),
    DAT(L"%08"),
    DAT(L"%09"),
    DAT(L"%0a"),
    DAT(L"%0b"),
    DAT(L"%0c"),
    DAT(L"%0d"),
    DAT(L"%0e"),
    DAT(L"%0f"),
    DAT(L"%10"),
    DAT(L"%11"),
    DAT(L"%12"),
    DAT(L"%13"),
    DAT(L"%14"),
    DAT(L"%15"),
    DAT(L"%16"),
    DAT(L"%17"),
    DAT(L"%18"),
    DAT(L"%19"),
    DAT(L"%1a"),
    DAT(L"%1b"),
    DAT(L"%1c"),
    DAT(L"%1d"),
    DAT(L"%1e"),
    DAT(L"%1f"),
    DAT(L"%20"),
    DAT(L"!"),
    DAT(L"%22"),
    DAT(L"%23"),
    DAT(L"%24"),
    DAT(L"%25"),
    DAT(L"%26"),
    DAT(L"%27"),
    DAT(L"("),
    DAT(L")"),
    DAT(L"*"),
    DAT(L"%2b"),
    DAT(L"%2c"),
    DAT(L"-"),
    DAT(L"."),
    DAT(L"%2f"),
    DAT(L"0"),
    DAT(L"1"),
    DAT(L"2"),
    DAT(L"3"),
    DAT(L"4"),
    DAT(L"5"),
    DAT(L"6"),
    DAT(L"7"),
    DAT(L"8"),
    DAT(L"9"),
    DAT(L"%3a"),
    DAT(L"%3b"),
    DAT(L"%3c"),
    DAT(L"%3d"),
    DAT(L"%3e"),
    DAT(L"%3f"),
    DAT(L"%40"),
    DAT(L"A"),
    DAT(L"B"),
    DAT(L"C"),
    DAT(L"D"),
    DAT(L"E"),
    DAT(L"F"),
    DAT(L"G"),
    DAT(L"H"),
    DAT(L"I"),
    DAT(L"J"),
    DAT(L"K"),
    DAT(L"L"),
    DAT(L"M"),
    DAT(L"N"),
    DAT(L"O"),
    DAT(L"P"),
    DAT(L"Q"),
    DAT(L"R"),
    DAT(L"S"),
    DAT(L"T"),
    DAT(L"U"),
    DAT(L"V"),
    DAT(L"W"),
    DAT(L"X"),
    DAT(L"Y"),
    DAT(L"Z"),
    DAT(L"%5b"),
    DAT(L"%5c"),
    DAT(L"%5d"),
    DAT(L"%5e"),
    DAT(L"_"),
    DAT(L"%60"),
    DAT(L"a"),
    DAT(L"b"),
    DAT(L"c"),
    DAT(L"d"),
    DAT(L"e"),
    DAT(L"f"),
    DAT(L"g"),
    DAT(L"h"),
    DAT(L"i"),
    DAT(L"j"),
    DAT(L"k"),
    DAT(L"l"),
    DAT(L"m"),
    DAT(L"n"),
    DAT(L"o"),
    DAT(L"p"),
    DAT(L"q"),
    DAT(L"r"),
    DAT(L"s"),
    DAT(L"t"),
    DAT(L"u"),
    DAT(L"v"),
    DAT(L"w"),
    DAT(L"x"),
    DAT(L"y"),
    DAT(L"z"),
    DAT(L"%7b"),
    DAT(L"%7c"),
    DAT(L"%7d"),
    DAT(L"%7e"),
    DAT(L"%7f"),
    DAT(L"%c2%80"),
    DAT(L"%c2%81"),
    DAT(L"%c2%82"),
    DAT(L"%c2%83"),
    DAT(L"%c2%84"),
    DAT(L"%c2%85"),
    DAT(L"%c2%86"),
    DAT(L"%c2%87"),
    DAT(L"%c2%88"),
    DAT(L"%c2%89"),
    DAT(L"%c2%8a"),
    DAT(L"%c2%8b"),
    DAT(L"%c2%8c"),
    DAT(L"%c2%8d"),
    DAT(L"%c2%8e"),
    DAT(L"%c2%8f"),
    DAT(L"%c2%90"),
    DAT(L"%c2%91"),
    DAT(L"%c2%92"),
    DAT(L"%c2%93"),
    DAT(L"%c2%94"),
    DAT(L"%c2%95"),
    DAT(L"%c2%96"),
    DAT(L"%c2%97"),
    DAT(L"%c2%98"),
    DAT(L"%c2%99"),
    DAT(L"%c2%9a"),
    DAT(L"%c2%9b"),
    DAT(L"%c2%9c"),
    DAT(L"%c2%9d"),
    DAT(L"%c2%9e"),
    DAT(L"%c2%9f"),
    DAT(L"%c2%a0"),
    DAT(L"%c2%a1"),
    DAT(L"%c2%a2"),
    DAT(L"%c2%a3"),
    DAT(L"%c2%a4"),
    DAT(L"%c2%a5"),
    DAT(L"%c2%a6"),
    DAT(L"%c2%a7"),
    DAT(L"%c2%a8"),
    DAT(L"%c2%a9"),
    DAT(L"%c2%aa"),
    DAT(L"%c2%ab"),
    DAT(L"%c2%ac"),
    DAT(L"%c2%ad"),
    DAT(L"%c2%ae"),
    DAT(L"%c2%af"),
    DAT(L"%c2%b0"),
    DAT(L"%c2%b1"),
    DAT(L"%c2%b2"),
    DAT(L"%c2%b3"),
    DAT(L"%c2%b4"),
    DAT(L"%c2%b5"),
    DAT(L"%c2%b6"),
    DAT(L"%c2%b7"),
    DAT(L"%c2%b8"),
    DAT(L"%c2%b9"),
    DAT(L"%c2%ba"),
    DAT(L"%c2%bb"),
    DAT(L"%c2%bc"),
    DAT(L"%c2%bd"),
    DAT(L"%c2%be"),
    DAT(L"%c2%bf"),
    DAT(L"%c3%80"),
    DAT(L"%c3%81"),
    DAT(L"%c3%82"),
    DAT(L"%c3%83"),
    DAT(L"%c3%84"),
    DAT(L"%c3%85"),
    DAT(L"%c3%86"),
    DAT(L"%c3%87"),
    DAT(L"%c3%88"),
    DAT(L"%c3%89"),
    DAT(L"%c3%8a"),
    DAT(L"%c3%8b"),
    DAT(L"%c3%8c"),
    DAT(L"%c3%8d"),
    DAT(L"%c3%8e"),
    DAT(L"%c3%8f"),
    DAT(L"%c3%90"),
    DAT(L"%c3%91"),
    DAT(L"%c3%92"),
    DAT(L"%c3%93"),
    DAT(L"%c3%94"),
    DAT(L"%c3%95"),
    DAT(L"%c3%96"),
    DAT(L"%c3%97"),
    DAT(L"%c3%98"),
    DAT(L"%c3%99"),
    DAT(L"%c3%9a"),
    DAT(L"%c3%9b"),
    DAT(L"%c3%9c"),
    DAT(L"%c3%9d"),
    DAT(L"%c3%9e"),
    DAT(L"%c3%9f"),
    DAT(L"%c3%a0"),
    DAT(L"%c3%a1"),
    DAT(L"%c3%a2"),
    DAT(L"%c3%a3"),
    DAT(L"%c3%a4"),
    DAT(L"%c3%a5"),
    DAT(L"%c3%a6"),
    DAT(L"%c3%a7"),
    DAT(L"%c3%a8"),
    DAT(L"%c3%a9"),
    DAT(L"%c3%aa"),
    DAT(L"%c3%ab"),
    DAT(L"%c3%ac"),
    DAT(L"%c3%ad"),
    DAT(L"%c3%ae"),
    DAT(L"%c3%af"),
    DAT(L"%c3%b0"),
    DAT(L"%c3%b1"),
    DAT(L"%c3%b2"),
    DAT(L"%c3%b3"),
    DAT(L"%c3%b4"),
    DAT(L"%c3%b5"),
    DAT(L"%c3%b6"),
    DAT(L"%c3%b7"),
    DAT(L"%c3%b8"),
    DAT(L"%c3%b9"),
    DAT(L"%c3%ba"),
    DAT(L"%c3%bb"),
    DAT(L"%c3%bc"),
    DAT(L"%c3%bd"),
    DAT(L"%c3%be"),
    DAT(L"%c3%bf")
};

UINT32 LimpetImportHostageKeyEx(
    __in UINT32 logicalDeviceNumber,
    __in_bcount(cbHostageBlob) PBYTE pbHostageBlob,
    __in UINT32 cbHostageBlob,
    __inout_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcKeyName,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcUriName
);

UINT32 ImportHostageBlobIfPresent(
    __in UINT32 logicalDeviceNumber
);

UINT32 LimpetReadURIFromTpm(
    __in UINT32 logicalDeviceNumber,
    __out_bcount_part(cbUriData, *pcbUriData) PBYTE pbUriData,
    __in UINT32 cbUriData,
    __out PUINT32 pcbUriData
);

UINT32 LimpetStoreURIInTpm(
    __in UINT32 logicalDeviceNumber,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* szURI,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
);

UINT32 ReadUriNameFromTpm(
    UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* name
);

UINT32 LimpetDestroyURIFromTpm(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
);


static UINT32 GetLogicalDeviceCert(
    __in UINT32 logicalDeviceNumber,
    __in_z LPCWSTR wcAlgorithm,
    __out AutoCloseCertificateContext* logicalDeviceCert,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    NTSTATUS status = ERROR_SUCCESS;
    UINT32 result = TPM_RC_SUCCESS;
    DWORD cbCert = 0;

    try
    {
        // Create the key name for the KSP
        std::wstring keyName(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetReadDeviceId(logicalDeviceNumber, &keyName[0])) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        keyName.resize(keyName.find(L'\0'));
        keyName += std::wstring(wcAlgorithm);
        keyName.push_back(L'\0');

        // Retrieve the idkCert from the key
        AutoCloseNcryptProvHandle hAlg;
        AutoCloseNcryptKeyHandle hKey;
        if (((status = NCryptOpenStorageProvider(&hAlg, MS_PLATFORM_KEY_STORAGE_PROVIDER, 0)) != ERROR_SUCCESS) ||
            ((status = NCryptOpenKey(hAlg.Get(), &hKey, &keyName[0], 0, NCRYPT_MACHINE_KEY_FLAG)) != ERROR_SUCCESS) ||
            ((status = NCryptGetProperty(hKey.Get(), NCRYPT_CERTIFICATE_PROPERTY, NULL, 0, &cbCert, 0)) != ERROR_SUCCESS))
        {
            throw TPM_RC_FAILURE;
        }
        std::vector<BYTE> cert(cbCert);
        if ((status = NCryptGetProperty(hKey.Get(), NCRYPT_CERTIFICATE_PROPERTY, &cert[0], (DWORD)cert.size(), &cbCert, 0)) != ERROR_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }

        // Open the cert and return the context
        logicalDeviceCert->reset(CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, &cert[0], cbCert));
        if (logicalDeviceCert->get() == NULL)
        {
            throw TPM_RC_FAILURE;
        }

        if ((wcName != nullptr) &&
            (StringCchCopyW(wcName, LIMPET_STRING_SIZE, &keyName[0]) != S_OK))
        {
            throw TPM_RC_FAILURE;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetSanitizeString(
    __in_ecount_z(LIMPET_STRING_SIZE) const WCHAR* szHmac,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* szHmacClean
)
{
    DWORD indexIn = 0;
    DWORD indexOut = 0;

    while (szHmac[indexIn] != '\0')
    {
        if ((indexIn >= LIMPET_STRING_SIZE) ||
            (indexOut >= LIMPET_STRING_SIZE) ||
            (szHmac[indexIn] > 0xff) ||
            (FAILED(StringCchCopyW(&szHmacClean[indexOut], LIMPET_STRING_SIZE - indexOut, urlEncoding[szHmac[indexIn]].encoding))))
        {
            return TPM_RC_FAILURE;
        }
        indexOut += (DWORD)(urlEncoding[szHmac[indexIn]].numberOfChars);
        indexIn++;
    }

    return TPM_RC_SUCCESS;
}

UINT32 LimpetObjNameToString(
    __in_bcount(nameLen) BYTE* name,
    __in UINT16 nameLen,
    __out_z WCHAR* nameStr
)
{
    UINT32 result = TPM_RC_SUCCESS;
    WCHAR wcEncodedName[LIMPET_STRING_SIZE] = L"";
    DWORD cchEncodedName = LIMPET_STRING_SIZE;

    try
    {
        // Encode the device name
        if (CryptBinaryToStringW(name, nameLen, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, wcEncodedName, &cchEncodedName) == FALSE)
        {
            throw TPM_RC_FAILURE;
        }

        // Format it for output
        if ((result = LimpetSanitizeString(wcEncodedName, nameStr)) != TPM_RC_SUCCESS)
        {
            throw result;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }

    return result;
}

UINT32 LimpetInitialize()
{
    _cpri__RngStartup();
    _cpri__HashStartup();
    _cpri__RsaStartup();
    _cpri__SymStartup();
    PlattformRetrieveAuthValues();
    return TPM_RC_SUCCESS;
}

UINT32 LimpetDestroy()
{
    _cpri__PlatformRelease();
    return TPM_RC_SUCCESS;
}

UINT32 LimpetGetTpmKey2BPubFromCert(
    __in_bcount(cbCert) PBYTE pbCert,
    UINT32 cbCert,
    __out_bcount_part(cbIdKeyPub, *pcbIdKeyPub) PBYTE pbIdKeyPub,
    __in UINT32 cbIdKeyPub,
    __out PUINT32 pcbIdKeyPub,
    __out_bcount_opt(SHA256_DIGEST_SIZE + sizeof(UINT16)) PBYTE idKeyName
)
{
    UINT32 result = TPM_RC_SUCCESS;
    PCCERT_CONTEXT cert = NULL;
    PCRYPT_DATA_BLOB  pTpmPublic = NULL;
    DWORD cbTpmPublic = 0;
    PBYTE buffer = NULL;
    INT32 size = 0;
    TPM2B_PUBLIC certKey = { 0 };

    try
    {
        // Open the cert so we can copy things out
        if ((cert = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, pbCert, cbCert)) == NULL)
        {
            throw TPM_RC_FAILURE;
        }

        // Iterate through the extensions and find the TPM public key in the cert request, not having that is fatal.
        for (UINT32 n = 0; n < cert->pCertInfo->cExtension; n++)
        {
            if ((!strcmp(cert->pCertInfo->rgExtension[n].pszObjId, szOID_SUBJECT_KEY_IDENTIFIER)) &&
                (cert->pCertInfo->rgExtension[0].fCritical == FALSE) &&
                (CryptDecodeObjectEx(X509_ASN_ENCODING,
                    szOID_SUBJECT_KEY_IDENTIFIER,
                    cert->pCertInfo->rgExtension[0].Value.pbData,
                    cert->pCertInfo->rgExtension[0].Value.cbData,
                    CRYPT_DECODE_ALLOC_FLAG,
                    NULL,
                    &pTpmPublic,
                    &cbTpmPublic)))
            {
                break;
            }
        }
        if ((pTpmPublic == NULL) || (cbTpmPublic == 0))
        {
            throw TPM_RC_FAILURE;
        }

        // Re-hydrate the TPM public key to calculate the name
        buffer = (PBYTE)pTpmPublic->pbData;
        size = (INT32)pTpmPublic->cbData;
        if (TPM2B_PUBLIC_Unmarshal(&certKey, &buffer, &size, FALSE) != TPM_RC_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }

        // Marshal the public key out
        buffer = pbIdKeyPub;
        size = cbIdKeyPub;
        *pcbIdKeyPub = TPM2B_PUBLIC_Marshal(&certKey, &buffer, &size);

        // Copy out the name if requested
        if (idKeyName != NULL)
        {
            TPM2B_NAME certKeyName = { 0 };
            ObjectComputeName(&certKey.t.publicArea, &certKeyName);
            MemoryCopy(idKeyName, certKeyName.t.name, certKeyName.t.size, SHA256_DIGEST_SIZE + sizeof(UINT16));
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    // Cleanup
    if (pTpmPublic != NULL)
    {
        LocalFree(pTpmPublic);
        pTpmPublic = NULL;
    }
    if (cert != NULL)
    {
        CertFreeCertificateContext(cert);
        cert = NULL;
    }
    return result;
}

UINT32 LimpetGetTpmKey2BPub(
    __in UINT32 tpmKeyHandle,
    __out_bcount_part(cbPubKey, *pcbPubKey) PBYTE pbPubKey,
    __in UINT32 cbPubKey,
    __out PUINT32 pcbPubKey,
    __out_bcount_opt(SHA256_DIGEST_SIZE + sizeof(UINT16)) PBYTE pubKeyName
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        ReadPublic_In readPublic;
    } tpm_in;
    union
    {
        ReadPublic_Out readPublic;
    } tpm_out;

    try
    {
        // Read the public definition to get the name
        INITIALIZE_CALL_BUFFERS(TPM2_ReadPublic, &tpm_in.readPublic, &tpm_out.readPublic);
        parms.objectTableIn[TPM2_ReadPublic_HdlIn_PublicKey].generic.handle = tpmKeyHandle;
        LIMPET_TPM_CALL(FALSE, TPM2_ReadPublic);

        // Marshal the TPM2B_Public structure out
        buffer = pbPubKey;
        size = cbPubKey;
        *pcbPubKey = TPM2B_PUBLIC_Marshal(&tpm_out.readPublic.outPublic, &buffer, &size);
        if (pubKeyName != NULL)
        {
            MemoryCopy(pubKeyName, tpm_out.readPublic.name.t.name, tpm_out.readPublic.name.t.size, SHA256_DIGEST_SIZE + sizeof(UINT16));
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetReadDeviceId(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_DEVICE_ID_STRING_LENGTH) WCHAR* wcDeviceId
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        ReadPublic_In readPublic;
    } tpm_in;
    union
    {
        ReadPublic_Out readPublic;
    } tpm_out;
    HASH_STATE hash = { 0 };
    TPM_ALG_ID hashAlg = TPM_ALG_SHA256;
    std::vector<BYTE> deviceId(LIMPET_HARDWARE_DEVICE_ID_SIZE);

    try
    {
        // Read the SRK name which is the basis for our logical deviceIDs. This name is different on every TPM.
        INITIALIZE_CALL_BUFFERS(TPM2_ReadPublic, &tpm_in.readPublic, &tpm_out.readPublic);
        parms.objectTableIn[TPM2_ReadPublic_HdlIn_PublicKey].generic.handle = LIMPET_TPM20_SRK_HANDLE;
        LIMPET_TPM_CALL(FALSE, TPM2_ReadPublic);

        // Calculate the logical deviceID = (hashAlg || SHA256( LogicalDeviceNo || SRKName))
        if (result == TPM_RC_SUCCESS)
        {
            buffer = &deviceId[0];
            size = (INT32)deviceId.size();
            TPM_ALG_ID_Marshal(&hashAlg, &buffer, &size);
            CryptStartHash(hashAlg, &hash);
            CryptUpdateDigest(&hash, sizeof(logicalDeviceNumber), (BYTE*)&logicalDeviceNumber);
            CryptUpdateDigest2B(&hash, (TPM2B*)&tpm_out.readPublic.name);
            if ((size -= CryptCompleteHash(&hash, (UINT16)size, buffer)) < 0)
            {
                throw TPM_RC_SIZE;
            }
        }

        // Make it readable
        if ((result = LimpetObjNameToString(&deviceId[0], LIMPET_HARDWARE_DEVICE_ID_SIZE, wcDeviceId)) != TPM_RC_SUCCESS)
        {
            throw result;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

bool IsRegistryKeyPresent(const std::wstring& subKeyPath)
{
    HKEY subKey;
    LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, KEY_READ, &subKey);
    if (SUCCEEDED_WIN32(result))
    {
        RegCloseKey(subKey);
        subKey = nullptr;
        return true;
    }
    return false;
}

LSTATUS TryReadStringRegistryValue(const std::wstring& subKeyPath, const std::wstring& propName, std::wstring &propValue)
{
    DWORD dataSize = 0;
    LSTATUS status;
    if (!SUCCEEDED_WIN32(status = RegGetValue(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), propName.c_str(), RRF_RT_REG_SZ, NULL, NULL, &dataSize)))
    {
        return status;
    }

    propValue.resize(dataSize);
    if (!SUCCEEDED_WIN32(status = RegGetValue(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), propName.c_str(), RRF_RT_REG_SZ, NULL, &propValue[0], &dataSize)))
    {
        return status;
    }
    propValue.resize(propValue.find(L'\0'));

    return S_OK;
}

ULONG ReadDwordRegistryValue(const std::wstring &valueName, DWORD* value)
{
    ULONG nError;
    HKEY hKeyNtservice;
    DWORD dwValue = 0;
    DWORD cbData = sizeof(dwValue);

    if (value == nullptr)
    {
        return ERROR_INVALID_PARAMETER;
    }

    nError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_SUBPATH_DMCLIENT_NTSERVICE.c_str(), 0, KEY_READ, &hKeyNtservice);

    if (ERROR_SUCCESS != nError)
    {
        return nError;
    }

    nError = RegQueryValueEx(hKeyNtservice, valueName.c_str(), NULL, NULL, (LPBYTE)&dwValue, &cbData);
    if (ERROR_SUCCESS != nError)
    {
        return nError;
    }
    *value = dwValue;

    if (hKeyNtservice != nullptr)
    {
        RegCloseKey(hKeyNtservice);
        hKeyNtservice = nullptr;
    }
    return nError;
}

UINT32 TryTpmNvOperations(UINT32 logicalDeviceNumber)
{
    UINT32 result = TPM_RC_SUCCESS;

    try
    {
        // make slot to clean state.
        std::wstring uriName(LIMPET_STRING_SIZE, L'\0');
        if (ReadUriNameFromTpm(logicalDeviceNumber, &uriName[0]) == TPM_RC_SUCCESS)
        {
            if ((result = LimpetDestroyURIFromTpm(logicalDeviceNumber, &uriName[0])) != TPM_RC_SUCCESS)
            {
                return result;
            }
        }

        // write
        std::wstring uriTest = L"limpettest.org/limpet";
        result = LimpetStoreURIInTpm(logicalDeviceNumber, &uriTest[0], &uriName[0]);
        if (result != TPM_RC_SUCCESS)
        {
            return result;
        }

        // read
        std::string uriData(LIMPET_STRING_SIZE, '\0');
        UINT32 cbUriData;

        result = LimpetReadURIFromTpm(
            logicalDeviceNumber,
            (PBYTE)&uriData[0],
            LIMPET_STRING_SIZE,
            &cbUriData);

        if (result != TPM_RC_SUCCESS)
        {
            return result;
        }

        // destory
        result = LimpetDestroyURIFromTpm(logicalDeviceNumber, &uriName[0]);
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

LSTATUS SetRegistryValueWithDword(std::wstring subKeyPath, const std::wstring propName, const DWORD propValue)
{
    LONG lRet;
    HKEY hkey;

    if (FAILED_WIN32(lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL)))
    {
        return lRet;
    }

    lRet = RegSetValueEx(hkey, propName.c_str(), 0, REG_DWORD, (BYTE*)&propValue, sizeof(DWORD));
    if (hkey != nullptr)
    {
        RegCloseKey(hkey);
        hkey = nullptr;
    }

    return lRet;
}

static bool UseRegistry(UINT32 logicalDeviceNumber)
{
    // If registry key value already present, it means either
    // customer explicitly opted in/out (OR)
    // TpmNvOperations set this registry value already.

    DWORD value = 0;
    LSTATUS result;
    if (SUCCEEDED_WIN32(ReadDwordRegistryValue(REG_KEY_NAME_USEREGISTRY, &value)))
    {
        if (value == 1)
        {
            return true;
        }
        return false;
    }

    if (TryTpmNvOperations(logicalDeviceNumber) == TPM_RC_SUCCESS)
    {
        if (FAILED_WIN32(result = SetRegistryValueWithDword(REG_KEY_SUBPATH_DMCLIENT_NTSERVICE, REG_KEY_NAME_USEREGISTRY, 0)))
        {
            throw result;
        }
    }
    else
    {
        if (FAILED_WIN32(result = SetRegistryValueWithDword(REG_KEY_SUBPATH_DMCLIENT_NTSERVICE, REG_KEY_NAME_USEREGISTRY, 1)))
        {
            throw result;
        }
        return true;
    }
    return false;
}

std::string WStringToString(const std::wstring data)
{
    int sizeNeeded = 0;
    size_t cchURI = data.size();

    // Convert the wstring to nvData and make sure that it is <= than LIMPET_STRING_SIZE
    if ((sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, data.c_str(), (int)cchURI, NULL, 0, nullptr, nullptr)) == 0)
    {
        throw TPM_RC_FAILURE;
    }

    std::string multiByteData(sizeNeeded, '\0');
    if ((sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, data.c_str(), (int)cchURI, &multiByteData[0], (int)multiByteData.size(), nullptr, nullptr)) == 0)
    {
        throw TPM_RC_FAILURE;
    }

    return multiByteData;
}

LSTATUS CreateStringRegistryKeyValue(const std::wstring& subKeyPath, const std::wstring& propName, const std::wstring& propValue)
{
    LONG lRet;
    HKEY hkey;
    if (!SUCCEEDED_WIN32(lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL)))
    {
        return lRet;
    }

    size_t propValueSize = (propValue.size() * sizeof(wchar_t)) + sizeof(wchar_t);
    lRet = RegSetValueEx(hkey, propName.c_str(), 0, RRF_RT_REG_SZ, (const BYTE*)propValue.c_str(), (DWORD)propValueSize);
    if (hkey != nullptr)
    {
        RegCloseKey(hkey);
        hkey = nullptr;
    }
    return lRet;
}

UINT32 LimpetStoreURIInRegistry(
    __in UINT32 logicalDeviceNumber,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* szURI,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    LONG lRet;

    // create uri registry value
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    const std::wstring uriValue{ szURI };
    if (!SUCCEEDED(lRet = CreateStringRegistryKeyValue(logicalDeviceNumberPath, L"uri", uriValue)))
    {
        return lRet;
    }

    if (wcName != nullptr)
    {
        wmemcpy_s(wcName, LIMPET_STRING_SIZE, FIXED_URI_OBJECT_NAME_FOR_REGISTRY.c_str(), FIXED_URI_OBJECT_NAME_FOR_REGISTRY.size());
    }
    return TPM_RC_SUCCESS;
}

#pragma prefast(suppress: __WARNING_EXCESSIVESTACKUSAGE, "WDGVSO:12767047 Code follows TCG/ISO/IEC standards.")
UINT32 LimpetStoreURIInTpm(
    __in UINT32 logicalDeviceNumber,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* szURI,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;

    union
    {
        NV_ReadPublic_In nv_ReadPublic;
        NV_DefineSpace_In nv_DefineSpace;
        NV_Write_In nv_Write;
    } tpm_in;
    union
    {
        NV_ReadPublic_Out nv_ReadPublic;
        NV_DefineSpace_Out nv_DefineSpace;
        NV_Write_Out nv_Write;
    } tpm_out;
    ANY_OBJECT nvIndex = { 0 };
    size_t cchURI = 0;
    SafeInt<UINT16> cbData = 0;
    int sizeNeeded = 0;

    try
    {
        // Convert the wstring to nvData and make sure that it is <= than LIMPET_STRING_SIZE
        if (((StringCchLengthW(szURI, LIMPET_STRING_SIZE, &cchURI) != S_OK) ||
            ((sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, szURI, (int)cchURI, NULL, 0, nullptr, nullptr)) == 0)))
        {
            throw TPM_RC_FAILURE;
        }
        std::vector<char> nvData(sizeNeeded);
        if ((sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, szURI, (int)cchURI, &nvData[0], (int)nvData.size(), nullptr, nullptr)) == 0)
        {
            throw TPM_RC_FAILURE;
        }

        // Define the NV storage space in the TPM
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_NV_DefineSpace, &tpm_in.nv_DefineSpace, &tpm_out.nv_DefineSpace);
        parms.objectTableIn[TPM2_NV_DefineSpace_HdlIn_AuthHandle].entity.handle = TPM_RH_OWNER;
        parms.objectTableIn[TPM2_NV_DefineSpace_HdlIn_AuthHandle].entity.authValue = g_StorageAuth;
        tpm_in.nv_DefineSpace.publicInfo.t.nvPublic.nvIndex = LIMPET_TPM20_PERSISTED_URI_INDEX + logicalDeviceNumber;
        tpm_in.nv_DefineSpace.publicInfo.t.nvPublic.nameAlg = TPM_ALG_SHA256;
        tpm_in.nv_DefineSpace.publicInfo.t.nvPublic.attributes.TPMA_NV_AUTHWRITE = SET;
        tpm_in.nv_DefineSpace.publicInfo.t.nvPublic.attributes.TPMA_NV_AUTHREAD = SET;
        tpm_in.nv_DefineSpace.publicInfo.t.nvPublic.attributes.TPMA_NV_NO_DA = SET;
        // This cast is OK, because StringCchLengthW above already made sure that  is never going to allow a string larger than LIMPET_STRING_SIZE
        tpm_in.nv_DefineSpace.publicInfo.t.nvPublic.dataSize = (UINT16)nvData.size();
        LIMPET_TPM_CALL(FALSE, TPM2_NV_DefineSpace);

        // Read the definition back to get the NV storage name
        INITIALIZE_CALL_BUFFERS(TPM2_NV_ReadPublic, &tpm_in.nv_ReadPublic, &tpm_out.nv_ReadPublic);
        parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex].generic.handle = LIMPET_TPM20_PERSISTED_URI_INDEX + logicalDeviceNumber;
        LIMPET_TPM_CALL(FALSE, TPM2_NV_ReadPublic);
        nvIndex = parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex];

        // Write the content into it as UTF8 string to save precious TPM NV space
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_NV_Write, &tpm_in.nv_Write, &tpm_in.nv_Write);
        parms.objectTableIn[TPM2_NV_Write_HdlIn_AuthHandle] = nvIndex;
        parms.objectTableIn[TPM2_NV_Write_HdlIn_NvIndex] = nvIndex;
        tpm_in.nv_Write.offset = 0;
        // This cast is OK, because StringCchLengthW above already made sure that  is never going to allow a string larger than LIMPET_STRING_SIZE
        cbData = nvData.size();
        tpm_in.nv_Write.data.t.size = cbData;
        MemoryCopy(tpm_in.nv_Write.data.t.buffer, &nvData[0], (UINT32)nvData.size(), sizeof(tpm_in.nv_Write.data.t.buffer));
        LIMPET_TPM_CALL(FALSE, TPM2_NV_Write);

        if (wcName != nullptr)
        {
            INITIALIZE_CALL_BUFFERS(TPM2_NV_ReadPublic, &tpm_in.nv_ReadPublic, &tpm_in.nv_ReadPublic);
            parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex].generic.handle = LIMPET_TPM20_PERSISTED_URI_INDEX + logicalDeviceNumber;
            LIMPET_TPM_CALL(FALSE, TPM2_NV_ReadPublic);
            nvIndex = parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex];
            result = LimpetObjNameToString(nvIndex.nv.name.t.name, nvIndex.nv.name.t.size, wcName);
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetStoreURI(
    __in UINT32 logicalDeviceNumber,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* szURI,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    if (UseRegistry(logicalDeviceNumber))
    {
        return LimpetStoreURIInRegistry(logicalDeviceNumber, szURI, wcName);
    }
    else
    {
        return LimpetStoreURIInTpm(logicalDeviceNumber, szURI, wcName);
    }
}

UINT32 ReadHmacNameFromRegistry(
    UINT logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* wcName)
{
    UINT32 result = TPM_RC_SUCCESS;
    std::wstring hmacName(LIMPET_STRING_SIZE, L'\0');
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    result = TryReadStringRegistryValue(logicalDeviceNumberPath, REG_KEY_NAME_HMACOBJECTNAME, hmacName);
    if (SUCCEEDED_WIN32(result) && (wcName != nullptr))
    {
        wmemcpy_s(wcName, LIMPET_STRING_SIZE, &hmacName[0], hmacName.size());
    }
    return result;
}

UINT32 ReadHmacNameFromTpm(
    UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* wcName)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        ReadPublic_In readPublic;
    } tpm_in;
    union
    {
        ReadPublic_Out readPublic;
    } tpm_out;
    TPM2B_NAME hmacKeyName = { 0 };

    try
    {
        hmacKeyName.t.size = 0;
        // Read the associated key. If it is not present then it is unused
        INITIALIZE_CALL_BUFFERS(TPM2_ReadPublic, &tpm_in.readPublic, &tpm_out.readPublic);
        parms.objectTableIn[TPM2_ReadPublic_HdlIn_PublicKey].generic.handle = LIMPET_TPM20_PERSISTED_KEY_INDEX + logicalDeviceNumber;
        LIMPET_TRY_TPM_CALL(FALSE, TPM2_ReadPublic);
        if (result == TPM_RC_SUCCESS)
        {
            hmacKeyName = tpm_out.readPublic.name;
        }

        if (hmacKeyName.t.size != 0)
        {
            if ((result = LimpetObjNameToString(hmacKeyName.t.name, hmacKeyName.t.size, wcName)) != TPM_RC_SUCCESS)
            {
                throw result;
            }
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }
    return result;
}

UINT32 LimpetReadHmacName(
    UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* name)
{
    if (UseRegistry(logicalDeviceNumber))
    {
        return ReadHmacNameFromRegistry(logicalDeviceNumber, name);
    }
    else
    {
        return ReadHmacNameFromTpm(logicalDeviceNumber, name);
    }
}

UINT32 ReadUriNameFromTpm(
    UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* name)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        NV_ReadPublic_In nv_ReadPublic;
    } tpm_in;
    union
    {
        NV_ReadPublic_Out nv_ReadPublic;
    } tpm_out;
    TPM2B_NAME uriName = { 0 };

    try
    {
        uriName.t.size = 0;
        // Read the associated key. If it is not present then it is unused
        INITIALIZE_CALL_BUFFERS(TPM2_NV_ReadPublic, &tpm_in.nv_ReadPublic, &tpm_out.nv_ReadPublic);
        parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex].generic.handle = LIMPET_TPM20_PERSISTED_URI_INDEX + logicalDeviceNumber;
        LIMPET_TRY_TPM_CALL(FALSE, TPM2_NV_ReadPublic);
        if (result == TPM_RC_SUCCESS)
        {
            uriName = tpm_out.nv_ReadPublic.nvName;
        }

        if (uriName.t.size != 0)
        {
            if ((result = LimpetObjNameToString(uriName.t.name, uriName.t.size, name)) != TPM_RC_SUCCESS)
            {
                throw result;
            }
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }
    return result;
}

UINT32 ReadUriNameFromRegistry(
    UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* wcName)
{
    UINT32 result = TPM_RC_SUCCESS;
    std::wstring uriName(LIMPET_STRING_SIZE, L'\0');
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    result = TryReadStringRegistryValue(logicalDeviceNumberPath, REG_KEY_NAME_IOTHUBSERVICEURI, uriName);
    if (SUCCEEDED_WIN32(result) && (wcName != nullptr))
    {
        wmemcpy_s(wcName, LIMPET_STRING_SIZE, FIXED_URI_OBJECT_NAME_FOR_REGISTRY.c_str(), FIXED_URI_OBJECT_NAME_FOR_REGISTRY.size());
    }
    return result;
}


UINT32 LimpetReadUriName(
    UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* name)
{
    if (UseRegistry(logicalDeviceNumber))
    {
        return ReadUriNameFromRegistry(logicalDeviceNumber, name);
    }
    else
    {
        return ReadUriNameFromTpm(logicalDeviceNumber, name);
    }
}

UINT32 LimpetReadURIFromRegistry(
    __in UINT32 logicalDeviceNumber,
    __out_bcount_part(cbUriData, *pcbUriData) PBYTE pbUriData,
    __in UINT32 cbUriData,
    __out PUINT32 pcbUriData
)
{
    LONG lRet;

    if (!SUCCEEDED_WIN32(lRet = ImportHostageBlobIfPresent(logicalDeviceNumber)))
    {
        return lRet;
    }

    // check for tpm slot registry key.
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    if (!IsRegistryKeyPresent(logicalDeviceNumberPath))
    {
        return TPM_RC_FAILURE;
    }

    std::wstring propValue;
    if (!SUCCEEDED_WIN32(lRet = TryReadStringRegistryValue(logicalDeviceNumberPath, REG_KEY_NAME_IOTHUBSERVICEURI, propValue)))
    {
        // Return HR as TPM_RC_FAILURE when tries to delete not existing registry value.
        if (lRet == ERROR_FILE_NOT_FOUND)
        {
            lRet = TPM_RC_FAILURE;
        }
        return lRet;
    }

    std::string multiBytePropValue = WStringToString(propValue);
    *pcbUriData = (UINT32)multiBytePropValue.size();
    if (((*pcbUriData) + (sizeof(BYTE))) > cbUriData)
    {
        return static_cast<UINT32>(E_INVALIDARG);;
    }
    *(pbUriData + *pcbUriData) = '\0';
    memcpy_s(pbUriData, cbUriData, &multiBytePropValue[0], *pcbUriData);

    return 0;
}

#pragma prefast(suppress: __WARNING_EXCESSIVESTACKUSAGE, "WDGVSO:12767047 Code follows TCG/ISO/IEC standards.")
UINT32 LimpetReadURIFromTpm(
    __in UINT32 logicalDeviceNumber,
    __out_bcount_part(cbUriData, *pcbUriData) PBYTE pbUriData,
    __in UINT32 cbUriData,
    __out PUINT32 pcbUriData
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        NV_ReadPublic_In nv_ReadPublic;
        NV_Read_In nv_Read;
    } tpm_in;
    union
    {
        NV_ReadPublic_Out nv_ReadPublic;
        NV_Read_Out nv_Read;
    } tpm_out;
    ANY_OBJECT nvIndex = { 0 };
    UINT16 nvDataSize = 0;
    AutoCloseCertificateContext idkCert;

    try
    {
        // Read the public definition to get the name
        INITIALIZE_CALL_BUFFERS(TPM2_NV_ReadPublic, &tpm_in.nv_ReadPublic, &tpm_out.nv_ReadPublic);
        parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex].generic.handle = LIMPET_TPM20_PERSISTED_URI_INDEX + logicalDeviceNumber;
        LIMPET_TRY_TPM_CALL(FALSE, TPM2_NV_ReadPublic);
        if (result == TPM_RC_SUCCESS)
        {
            nvIndex = parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex];
            nvDataSize = tpm_out.nv_ReadPublic.nvPublic.t.nvPublic.dataSize;

            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_NV_Read, &tpm_in.nv_Read, &tpm_out.nv_Read);
            parms.objectTableIn[TPM2_NV_Read_HdlIn_AuthHandle] = nvIndex;
            parms.objectTableIn[TPM2_NV_Read_HdlIn_NvIndex] = nvIndex;
            tpm_in.nv_Read.offset = 0;
            tpm_in.nv_Read.size = nvDataSize;
            LIMPET_TPM_CALL(FALSE, TPM2_NV_Read);

            *pcbUriData = tpm_out.nv_Read.data.t.size;
            if ((cbUriData >= tpm_out.nv_Read.data.t.size) && (pbUriData != nullptr))
            {
                MemoryCopy(pbUriData, tpm_out.nv_Read.data.t.buffer, tpm_out.nv_Read.data.t.size, cbUriData);
            }
            else if ((cbUriData != 0) && (pbUriData != nullptr))
            {
                throw TPM_RC_SIZE;
            }
        }

        // If nothing defined lets look into the assymetric certificates
        else if (((result = GetLogicalDeviceCert(logicalDeviceNumber, BCRYPT_RSA_ALGORITHM, &idkCert, NULL)) == TPM_RC_SUCCESS) ||
            ((result = GetLogicalDeviceCert(logicalDeviceNumber, BCRYPT_ECDSA_P256_ALGORITHM, &idkCert, NULL)) == TPM_RC_SUCCESS))
        {
            std::string subjectName(LIMPET_STRING_SIZE, '\0');
            if (!CertNameToStrA(X509_ASN_ENCODING, &idkCert.get()->pCertInfo->Subject, CERT_X500_NAME_STR, &subjectName[0], (DWORD)subjectName.capacity()))
            {
                throw TPM_RC_FAILURE;
            }
            subjectName.resize(subjectName.find('\0'));
            const std::string deviceIdToken("OU=");
            const std::string hostNameToken("DC=");
            const std::string separatorToken(",");
            size_t cursor;
            size_t length;
            if ((cursor = subjectName.find(hostNameToken)) == std::string::npos)
            {
                throw TPM_RC_FAILURE;
            }
            cursor += hostNameToken.size();
            length = subjectName.substr(cursor).find(separatorToken);
            std::string URI = subjectName.substr(cursor, length);

            if ((cursor = subjectName.find(deviceIdToken)) != std::string::npos)
            {
                cursor += deviceIdToken.size();
                length = subjectName.substr(cursor).find(separatorToken);
                URI += std::string("/") + subjectName.substr(cursor, length);
            }

            *pcbUriData = (UINT32)URI.size();
            if ((cbUriData >= (UINT32)URI.size()) && (pbUriData != nullptr))
            {
                MemoryCopy(pbUriData, &URI[0], (UINT32)URI.size(), cbUriData);
            }
            else if ((cbUriData != 0) && (pbUriData != nullptr))
            {
                throw TPM_RC_SIZE;
            }
        }

        // No such luck
        else
        {
            throw TPM_RC_FAILURE;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetReadURI(
    __in UINT32 logicalDeviceNumber,
    __out_bcount_part(cbUriData, *pcbUriData) PBYTE pbUriData,
    __in UINT32 cbUriData,
    __out PUINT32 pcbUriData
)
{
    if (UseRegistry(logicalDeviceNumber))
    {
        return LimpetReadURIFromRegistry(logicalDeviceNumber, pbUriData, cbUriData, pcbUriData);
    }
    else
    {
        return LimpetReadURIFromTpm(logicalDeviceNumber, pbUriData, cbUriData, pcbUriData);
    }
}

LSTATUS DeleteRegistryKeyValue(const std::wstring subKey, const std::wstring propName)
{
    LONG lRet;
    HKEY hkey;

    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey.c_str(), 0, KEY_ALL_ACCESS, &hkey);
    if (!SUCCEEDED(lRet))
    {
        if (lRet == ERROR_FILE_NOT_FOUND)
        {
            lRet = TPM_RC_SUCCESS;
        }
        return lRet;
    }

    if (!SUCCEEDED(lRet = RegDeleteValue(hkey, propName.c_str())))
    {
        // Return no error when tries to delete not existing registry value.
        if (lRet == ERROR_FILE_NOT_FOUND)
        {
            lRet = TPM_RC_SUCCESS;
        }
    }

    if (hkey != nullptr)
    {
        RegCloseKey(hkey);
        hkey = nullptr;
    }
    return lRet;
}


bool IsLazyLoadingEnabled()
{
    DWORD value = 0;
    if (SUCCEEDED_WIN32(ReadDwordRegistryValue(REG_KEY_NAME_LAZYLOADINGENABLED, &value)))
    {
        if (value == 1)
        {
            return true;
        }
    }
    return false;
}

bool IsHostageBlobPresent(const UINT32 logicalDeviceNumber, INT32 *cbhostageBlob)
{
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    ULONG nError;
    HKEY hKeyNtservice;

    nError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, logicalDeviceNumberPath.c_str(), 0, KEY_READ, &hKeyNtservice);

    // return false, if tpm slot registry does not exist.
    if (ERROR_BADKEY == nError || ERROR_FILE_NOT_FOUND == nError)
    {
        return false;
    }
    else if (ERROR_SUCCESS != nError)
    {
        return false;
    }

    // Now, check the hostage blob registry value exist
    DWORD cbData = 0;
    DWORD dwRegType = REG_BINARY;

    // Retrieve size of the blob
    nError = RegQueryValueEx(hKeyNtservice, REG_KEY_NAME_HOSTAGEBLOB.c_str(), NULL, &dwRegType, NULL, &cbData);
    if (ERROR_BADKEY == nError || ERROR_FILE_NOT_FOUND == nError)
    {
        return false;
    }
    else if (ERROR_SUCCESS != nError)
    {
        return false;
    }

    if (cbhostageBlob != nullptr)
    {
        *cbhostageBlob = (INT32)cbData;
    }

    if (hKeyNtservice != nullptr)
    {
        RegCloseKey(hKeyNtservice);
        hKeyNtservice = nullptr;
    }
    return true;
}


LSTATUS DeleteHostageBlob(UINT32 logicalDeviceNumber)
{
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    return DeleteRegistryKeyValue(logicalDeviceNumberPath, REG_KEY_NAME_HOSTAGEBLOB);
}

UINT32 DestoryHostageBlobIfPresent(UINT32 logicalDeviceNumber)
{
    UINT32 result = TPM_RC_SUCCESS;

    if (UseRegistry(logicalDeviceNumber) &&
        IsLazyLoadingEnabled() &&
        IsHostageBlobPresent(logicalDeviceNumber, nullptr))
    {
        result = DeleteHostageBlob(logicalDeviceNumber);
    }

    return result;
}

UINT32 LimpetDestroyURIFromRegistry(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    LONG lRet;

    if (!SUCCEEDED_WIN32(lRet = DestoryHostageBlobIfPresent(logicalDeviceNumber)))
    {
        return lRet;
    }

    // check for tpm slot registry key.
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    if (!IsRegistryKeyPresent(logicalDeviceNumberPath))
    {
        return TPM_RC_FAILURE;
    }

    std::wstring propValue;
    if (!SUCCEEDED_WIN32(lRet = TryReadStringRegistryValue(logicalDeviceNumberPath, REG_KEY_NAME_IOTHUBSERVICEURI, propValue)))
    {
        if (lRet == ERROR_FILE_NOT_FOUND)
        {
            return TPM_RC_FAILURE;
        }
    }

    if (wcName != nullptr)
    {
        wmemcpy_s(wcName, LIMPET_STRING_SIZE, FIXED_URI_OBJECT_NAME_FOR_REGISTRY.c_str(), FIXED_URI_OBJECT_NAME_FOR_REGISTRY.size());
    }

    return DeleteRegistryKeyValue(logicalDeviceNumberPath, REG_KEY_NAME_IOTHUBSERVICEURI);
}

UINT32 LimpetDestroyURIFromTpm(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        NV_ReadPublic_In nv_ReadPublic;
        NV_UndefineSpace_In nv_UndefineSpace;
    } tpm_in;
    union
    {
        NV_ReadPublic_Out nv_ReadPublic;
        NV_UndefineSpace_Out nv_UndefineSpace;
    } tpm_out;
    ANY_OBJECT nvIndex = { 0 };

    try
    {
        // Read the public information to get the name
        INITIALIZE_CALL_BUFFERS(TPM2_NV_ReadPublic, &tpm_in.nv_ReadPublic, &tpm_out.nv_ReadPublic);
        parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex].generic.handle = LIMPET_TPM20_PERSISTED_URI_INDEX + logicalDeviceNumber;
        LIMPET_TPM_CALL(FALSE, TPM2_NV_ReadPublic);
        nvIndex = parms.objectTableIn[TPM2_NV_ReadPublic_HdlIn_NvIndex];

        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_NV_UndefineSpace, &tpm_in.nv_UndefineSpace, &tpm_out.nv_UndefineSpace);
        parms.objectTableIn[TPM2_NV_UndefineSpace_HdlIn_AuthHandle].entity.handle = TPM_RH_OWNER;
        parms.objectTableIn[TPM2_NV_UndefineSpace_HdlIn_AuthHandle].entity.authValue = g_StorageAuth;
        parms.objectTableIn[TPM2_NV_UndefineSpace_HdlIn_NvIndex] = nvIndex;
        LIMPET_TPM_CALL(FALSE, TPM2_NV_UndefineSpace);
        nvIndex = parms.objectTableIn[TPM2_NV_UndefineSpace_HdlIn_NvIndex];

        if ((wcName != nullptr) &&
            ((result = LimpetObjNameToString(nvIndex.nv.name.t.name, nvIndex.nv.name.t.size, wcName)) != TPM_RC_SUCCESS))
        {
            throw result;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetDestroyURI(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    if (UseRegistry(logicalDeviceNumber))
    {
        return LimpetDestroyURIFromRegistry(logicalDeviceNumber, wcName);
    }
    else
    {

        return LimpetDestroyURIFromTpm(logicalDeviceNumber, wcName);
    }
}

LSTATUS SetRegistryWithBinaryData(std::wstring subKeyPath, const std::wstring propName, BYTE* data, DWORD cbData)
{
    LONG lRet;
    HKEY hkey;

    if (!SUCCEEDED_WIN32(lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL)))
    {
        return lRet;
    }
    lRet = RegSetValueEx(hkey, propName.c_str(), 0, REG_BINARY, data, cbData);
    if (hkey != nullptr)
    {
        RegCloseKey(hkey);
        hkey = nullptr;
    }

    return lRet;
}

#pragma prefast(suppress: __WARNING_EXCESSIVESTACKUSAGE, "WDGVSO:12767047 Code follows TCG/ISO/IEC standards.")
UINT32 LimpetCreateHmacKey(
    __in UINT32 logicalDeviceNumber,
    __in_bcount(cbKey) PBYTE pbKey,
    __in UINT32 cbKey,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        Create_In create;
        Load_In load;
        EvictControl_In evictControl;
        FlushContext_In flushContext;
    } tpm_in;
    union
    {
        Create_Out create;
        Load_Out load;
        EvictControl_Out evictControl;
        FlushContext_Out flushContext;
    }tpm_out;
    std::unique_ptr<ANY_OBJECT> hmacKey(new ANY_OBJECT);
    SafeInt<UINT16> keyLen = 0;

    try
    {
        // Ensure that the key size is not causing an integer overflow
        keyLen = cbKey;

        // Create the HMAC key, or better import it after Base64 decoding it
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_Create, &tpm_in.create, &tpm_out.create);
        parms.objectTableIn[TPM2_Create_HdlIn_ParentHandle].generic.handle = TPM_20_SRK_HANDLE;
        tpm_in.create.inPublic.t.publicArea.type = TPM_ALG_KEYEDHASH;
        tpm_in.create.inPublic.t.publicArea.nameAlg = TPM_ALG_SHA256;
        tpm_in.create.inPublic.t.publicArea.objectAttributes.userWithAuth = 1;
        tpm_in.create.inPublic.t.publicArea.objectAttributes.noDA = 1;
        tpm_in.create.inPublic.t.publicArea.objectAttributes.sign = 1;
        tpm_in.create.inPublic.t.publicArea.parameters.keyedHashDetail.scheme.scheme = TPM_ALG_HMAC;
        tpm_in.create.inPublic.t.publicArea.parameters.keyedHashDetail.scheme.details.hmac.hashAlg = TPM_ALG_SHA256;
        tpm_in.create.inSensitive.t.sensitive.data.t.size = keyLen;
        MemoryCopy(tpm_in.create.inSensitive.t.sensitive.data.t.buffer, pbKey, keyLen, sizeof(tpm_in.create.inSensitive.t.sensitive.data.t.buffer));
        LIMPET_TPM_CALL(FALSE, TPM2_Create);

        // Copy the HMAC key object out
        hmacKey->obj.publicArea = tpm_out.create.outPublic;
        hmacKey->obj.privateArea = tpm_out.create.outPrivate;

        if (UseRegistry(logicalDeviceNumber))
        {
            const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);

            std::wstring localhmacObjectName(LIMPET_STRING_SIZE, L'\0');
            BYTE hmacBuffer[2048];
            INT32 cbhmacBuffer = 2048;
            INT32 actualDataSize = 0;

            // write public key in registry
            for (INT32 i = 0; i < cbhmacBuffer; i++)
            {
                hmacBuffer[i] = '\0';
            }
            buffer = hmacBuffer;
            size = cbhmacBuffer;
            actualDataSize = TPM2B_PUBLIC_Marshal(&hmacKey->obj.publicArea, &buffer, &size);

            if (!SUCCEEDED_WIN32(result = SetRegistryWithBinaryData(logicalDeviceNumberPath, REG_KEY_NAME_HMACPUBLICKEYNAME, (BYTE*)hmacBuffer, (DWORD)actualDataSize)))
            {
                return result;
            }

            // write the private key in registry.
            cbhmacBuffer = 2048;
            for (INT32 i = 0; i < cbhmacBuffer; i++)
            {
                hmacBuffer[i] = '\0';
            }
            buffer = hmacBuffer;
            size = cbhmacBuffer;
            actualDataSize = TPM2B_PRIVATE_Marshal(&hmacKey->obj.privateArea, &buffer, &size);
            if ((result = SetRegistryWithBinaryData(logicalDeviceNumberPath, REG_KEY_NAME_HMACPRIVATEKEYNAME, (BYTE*)hmacBuffer, (DWORD)actualDataSize)) != 0)
            {
                return result;
            }

            //write hmac object name in the registry.
            result = CreateStringRegistryKeyValue(logicalDeviceNumberPath, REG_KEY_NAME_HMACOBJECTNAME, FIXED_HMAC_OBJECT_NAME_FOR_REGISTRY);

            // Return the fixed hmac object name.
            if (wcName != nullptr)
            {
                wmemcpy_s(wcName, LIMPET_STRING_SIZE, FIXED_HMAC_OBJECT_NAME_FOR_REGISTRY.c_str(), FIXED_HMAC_OBJECT_NAME_FOR_REGISTRY.size());
            }
        }
        else
        {
            // Load the HMAC key object
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_Load, &tpm_in.load, &tpm_out.load);
            parms.objectTableIn[TPM2_Load_HdlIn_ParentHandle].generic.handle = TPM_20_SRK_HANDLE;
            parms.objectTableOut[TPM2_Load_HdlOut_ObjectHandle] = *hmacKey; // Copy the key in to be updated
            tpm_in.load.inPublic = hmacKey->obj.publicArea;
            tpm_in.load.inPrivate = hmacKey->obj.privateArea;
            LIMPET_TPM_CALL(FALSE, TPM2_Load);

            // Copy the updated HMAC back out. This one has a valid handle now
            *hmacKey = parms.objectTableOut[TPM2_Load_HdlOut_ObjectHandle];

            // Persist the key in TPM NV storage and it will never ever see the light of day
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_EvictControl, &tpm_in.evictControl, &tpm_out.evictControl);
            parms.objectTableIn[TPM2_EvictControl_HdlIn_Auth].entity.handle = TPM_RH_OWNER;
            parms.objectTableIn[TPM2_EvictControl_HdlIn_Auth].entity.authValue = g_StorageAuth;
            parms.objectTableIn[TPM2_EvictControl_HdlIn_ObjectHandle] = *hmacKey;
            tpm_in.evictControl.persistentHandle = LIMPET_TPM20_PERSISTED_KEY_INDEX + logicalDeviceNumber;
            LIMPET_TPM_CALL(FALSE, TPM2_EvictControl);

            // Return the HMAC key objects name
            if (wcName != nullptr)
            {
                result = LimpetObjNameToString(hmacKey->obj.name.t.name, hmacKey->obj.name.t.size, wcName);
            }
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

LSTATUS ReadRegistryBinaryData(std::wstring subKeyPath, const std::wstring propName, BYTE* data, INT32* cbData)
{
    LONG lRet;
    HKEY hkey;
    DWORD dataSize = 0;

    if (!SUCCEEDED_WIN32(lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, KEY_QUERY_VALUE, &hkey)))
    {
        return lRet;
    }
    else if (!SUCCEEDED_WIN32(lRet = RegQueryValueEx(hkey, propName.c_str(), 0, 0, NULL, &dataSize)))
    {
    }
    else if ((*cbData) < (INT32)dataSize)
    {
        lRet = ERROR_NOT_ENOUGH_MEMORY;
    }
    else if (!SUCCEEDED_WIN32(lRet = RegQueryValueEx(hkey, propName.c_str(), 0, 0, (LPBYTE)data, &dataSize)))
    {
    }
    else
    {
        *cbData = (INT32)dataSize;
    }

    if (hkey != nullptr)
    {
        RegCloseKey(hkey);
        hkey = nullptr;
    }
    return lRet;
}

UINT32 LimpetEvictHmacKey(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        ReadPublic_In readPublic;
        EvictControl_In evictControl;
        Load_In load;
        FlushContext_In flushContext;
    } tpm_in;
    union
    {
        ReadPublic_Out readPublic;
        EvictControl_Out evictControl;
        Load_Out load;
        FlushContext_Out flushContext;
    } tpm_out;

    std::unique_ptr<ANY_OBJECT> hmacKey(new ANY_OBJECT);

    try
    {
        if (UseRegistry(logicalDeviceNumber))
        {
            if (!SUCCEEDED_WIN32(result = DestoryHostageBlobIfPresent(logicalDeviceNumber)))
            {
                return result;
            }

            const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);

            if (!SUCCEEDED_WIN32(result = DeleteRegistryKeyValue(logicalDeviceNumberPath, REG_KEY_NAME_HMACPUBLICKEYNAME)))
            {
                return result;
            }

            if (!SUCCEEDED_WIN32(result = DeleteRegistryKeyValue(logicalDeviceNumberPath, REG_KEY_NAME_HMACPRIVATEKEYNAME)))
            {
                return result;
            }

            std::wstring hmacName(LIMPET_STRING_SIZE, L'\0');
            result = TryReadStringRegistryValue(logicalDeviceNumberPath, REG_KEY_NAME_HMACOBJECTNAME, hmacName);

            if (wcName != nullptr)
            {
                wmemcpy_s(wcName, LIMPET_STRING_SIZE, &hmacName[0], hmacName.size());
            }

            if (!SUCCEEDED_WIN32(result = DeleteRegistryKeyValue(logicalDeviceNumberPath, REG_KEY_NAME_HMACOBJECTNAME)))
            {
                return result;
            }
        }
        else
        {
            // Read the public portion to get the name
            INITIALIZE_CALL_BUFFERS(TPM2_ReadPublic, &tpm_in.readPublic, &tpm_out.readPublic);
            parms.objectTableIn[TPM2_ReadPublic_HdlIn_PublicKey].obj.handle = LIMPET_TPM20_PERSISTED_KEY_INDEX + logicalDeviceNumber;
            LIMPET_TPM_CALL(FALSE, TPM2_ReadPublic);
            *hmacKey = parms.objectTableIn[TPM2_ReadPublic_HdlIn_PublicKey];

            // Evict the key from TPM NV. This will irretrievably destroy it. Note the key is not returned!
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_EvictControl, &tpm_in.evictControl, &tpm_in.evictControl);
            parms.objectTableIn[TPM2_EvictControl_HdlIn_Auth].entity.handle = TPM_RH_OWNER;
            parms.objectTableIn[TPM2_EvictControl_HdlIn_Auth].entity.authValue = g_StorageAuth;
            parms.objectTableIn[TPM2_EvictControl_HdlIn_ObjectHandle] = *hmacKey;
            tpm_in.evictControl.persistentHandle = LIMPET_TPM20_PERSISTED_KEY_INDEX + logicalDeviceNumber;
            LIMPET_TPM_CALL(FALSE, TPM2_EvictControl);

            // Tell the world which key we just whacked
            if (wcName != nullptr)
            {
                result = LimpetObjNameToString(hmacKey->obj.name.t.name, hmacKey->obj.name.t.size, wcName);
            }
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetIsLogicalDevicePresent(
    __in UINT32 logicalDeviceNumber,
    __out PBOOL exist
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        ReadPublic_In readPublic;
        EvictControl_In evictControl;
    } tpm_in;
    union
    {
        ReadPublic_Out readPublic;
        EvictControl_Out evictControl;
    } tpm_out;
    ANY_OBJECT hmacKey = { 0 };

    // Read the public portion to get the name
    INITIALIZE_CALL_BUFFERS(TPM2_ReadPublic, &tpm_in.readPublic, &tpm_out.readPublic);
    parms.objectTableIn[TPM2_ReadPublic_HdlIn_PublicKey].obj.handle = LIMPET_TPM20_PERSISTED_KEY_INDEX + logicalDeviceNumber;
    LIMPET_TRY_TPM_CALL(FALSE, TPM2_ReadPublic);

    *exist = (result == TPM_RC_SUCCESS);

    return 0;
}

UINT32 LimpetSymmetricPayloadProtection(
    __in bool encrypt,
    __in_bcount(MAX_AES_KEY_BYTES) PBYTE aes128Key,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* wcfileName
)
{
    //NTSTATUS status = ERROR_SUCCESS;
    UINT32 result = TPM_RC_SUCCESS;
    DWORD dataSize = 0;

    try
    {
        // Open the file for reading and get the size of it
        AutoCloseHandle hDataFile(CreateFileW(wcfileName,
            GENERIC_ALL,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr));
        if ((hDataFile.Get() == INVALID_HANDLE_VALUE) ||
            ((dataSize = GetFileSize(hDataFile.Get(), nullptr)) <= 0))
        {
            throw TPM_RC_FAILURE;
        }

        // Process the entire file in 16 byte blocks so we don't have to load the entire file into memory
        TPM2B_IV iv = { 0 };
        iv.t.size = MAX_AES_BLOCK_SIZE_BYTES;
        for (UINT32 n = 0; n < dataSize; n += 16)
        {
            DWORD readData = 0;
            DWORD processedData = 0;
            std::vector<BYTE> dataBuffer(MAX_AES_BLOCK_SIZE_BYTES);
            SetFilePointer(hDataFile.Get(), n, nullptr, FILE_BEGIN);
            if (!ReadFile(hDataFile.Get(), &dataBuffer[0], min((DWORD)dataBuffer.size(), (dataSize - n)), &readData, nullptr))
            {
                throw TPM_RC_FAILURE;
            }

            if (encrypt)
            {

                CryptSymmetricEncrypt(&dataBuffer[0], TPM_ALG_AES, AES_KEY_SIZES_BITS, TPM_ALG_CFB, aes128Key, &iv, (ULONG)dataBuffer.size(), &dataBuffer[0]);
            }
            else
            {
                CryptSymmetricDecrypt(&dataBuffer[0], TPM_ALG_AES, AES_KEY_SIZES_BITS, TPM_ALG_CFB, aes128Key, &iv, (ULONG)dataBuffer.size(), &dataBuffer[0]);
            }

            if ((SetFilePointer(hDataFile.Get(), n, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ||
                (!WriteFile(hDataFile.Get(), &dataBuffer[0], readData, &processedData, nullptr)))
            {
                throw TPM_RC_FAILURE;
            }
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    // Cleanup
    return result;
}

UINT32 LimpetReadOrCreateIdentityKeyCert(
    __in UINT32 logicalDeviceNumber,
    __in_z LPCWSTR wcAlgorithm,
    __out_bcount_part(cbCert, *pcbCert) PBYTE pbCert,
    __in UINT32 cbCert,
    __out PUINT32 pcbCert,
    __out_bcount(SHA1_DIGEST_SIZE) PBYTE certThumbPrint,
    __out_bcount_part(cbIdKeyPub, *pcbIdKeyPub) PBYTE pbIdKeyPub,
    __in UINT32 cbIdKeyPub,
    __out PUINT32 pcbIdKeyPub,
    __out_bcount(SHA256_DIGEST_SIZE + sizeof(UINT16)) PBYTE idKeyName,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    NTSTATUS status = ERROR_SUCCESS;
    UINT32 result = TPM_RC_SUCCESS;
    DWORD returnedSize = 0;
    PVOID LimpetTbsHandle = g_hTbs; // We remember this here so we can restore the context later when we are done
    PVOID NCryptTbsHandle = NULL;
    UINT32 NCryptTpmKeyHandle = 0;

    try
    {
        // Create the key name for the KSP
        std::wstring keyName(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetReadDeviceId(logicalDeviceNumber, &keyName[0])) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        keyName.resize(keyName.find(L'\0'));
        keyName += std::wstring(wcAlgorithm);
        keyName.push_back(L'\0');

        // Open PCPKSP
        AutoCloseNcryptProvHandle hAlg;
        if ((status = NCryptOpenStorageProvider(&hAlg, MS_PLATFORM_KEY_STORAGE_PROVIDER, 0)) != ERROR_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }

        // Open the key if it exists
        AutoCloseNcryptKeyHandle hKey;
        if ((status = NCryptOpenKey(hAlg.Get(), &hKey, &keyName[0], 0, NCRYPT_MACHINE_KEY_FLAG)) != ERROR_SUCCESS)
        {
            // The key does not exist. We have to start fresh.
            CERT_NAME_BLOB nameBlob = { 0 };
            CRYPT_KEY_PROV_INFO keyProvInfo = { 0 };
            SYSTEMTIME startTime = { 0 };
            SYSTEMTIME endTime = { 0 };
            FILETIME fileTime = { 0 };
            LARGE_INTEGER timeData = { 0 };
            DWORD createFlags = NCRYPT_MACHINE_KEY_FLAG | NCRYPT_OVERWRITE_KEY_FLAG;
            CRYPT_DATA_BLOB tpmPublic = { 0 };
            std::string identifier{ szOID_SUBJECT_KEY_IDENTIFIER };
            CERT_EXTENSION certExtension[] = { { &identifier[0], FALSE, { 0, NULL } } };
            CERT_EXTENSIONS certExtensions = { 1, certExtension };

            // It does not exists yet, create it
            if (((status = NCryptCreatePersistedKey(hAlg.Get(), &hKey, wcAlgorithm, &keyName[0], 0, createFlags)) != ERROR_SUCCESS) ||
                ((status = NCryptFinalizeKey(hKey.Get(), 0)) != ERROR_SUCCESS))
            {
                throw TPM_RC_FAILURE;
            }

            // Create the subject name
            std::wstring subjectName(LIMPET_STRING_SIZE, L'\0');
            if (FAILED(StringCchPrintfW(&subjectName[0], subjectName.capacity(), L"CN=Limpet%02d", logicalDeviceNumber)) ||
                (!CertStrToNameW(X509_ASN_ENCODING, &subjectName[0], CERT_OID_NAME_STR, NULL, NULL, &nameBlob.cbData, NULL)))
            {
                throw TPM_RC_FAILURE;
            }
            subjectName.resize(subjectName.find(L'\0'));
            std::vector<BYTE> encodedSubjectName(nameBlob.cbData);
            nameBlob.pbData = &encodedSubjectName[0];
            if (!CertStrToNameW(X509_ASN_ENCODING, &subjectName[0], CERT_OID_NAME_STR, NULL, nameBlob.pbData, &nameBlob.cbData, NULL))
            {
                throw TPM_RC_FAILURE;
            }

            // Lets hijack the TBS context and TPM handle from NCrypt so we can interact with the key
            if (((status = NCryptGetProperty(hAlg.Get(), NCRYPT_PCP_PLATFORMHANDLE_PROPERTY, (PBYTE)&NCryptTbsHandle, sizeof(NCryptTbsHandle), &returnedSize, 0)) != ERROR_SUCCESS) ||
                ((status = NCryptGetProperty(hKey.Get(), NCRYPT_PCP_PLATFORMHANDLE_PROPERTY, (PBYTE)&NCryptTpmKeyHandle, sizeof(NCryptTpmKeyHandle), &returnedSize, 0)) != ERROR_SUCCESS))
            {
                throw TPM_RC_FAILURE;
            }

            // Switch Urchin to the PCPKSP owned TBS context and read the the IDKPub
            g_hTbs = NCryptTbsHandle;
            if ((result = LimpetGetTpmKey2BPub(NCryptTpmKeyHandle, pbIdKeyPub, cbIdKeyPub, pcbIdKeyPub, idKeyName)) != TPM_RC_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }

            // Add the TPM2B_Public to the cert
            tpmPublic.cbData = *pcbIdKeyPub;
            tpmPublic.pbData = pbIdKeyPub;

            // Encode the key attributes extension that will contain the entire TPM2B_Public so we can later calculate the TPM key name
            if (!CryptEncodeObjectEx(X509_ASN_ENCODING,
                szOID_SUBJECT_KEY_IDENTIFIER,
                &tpmPublic,
                CRYPT_ENCODE_ALLOC_FLAG,
                NULL,
                &certExtension[0].Value.pbData,
                &certExtension[0].Value.cbData))
            {
                throw TPM_RC_FAILURE;
            }

            // Link the cert to the private key on this system
            keyProvInfo.pwszContainerName = &keyName[0];
            keyProvInfo.pwszProvName = const_cast<WCHAR*>(MS_PLATFORM_KEY_STORAGE_PROVIDER);
            keyProvInfo.dwFlags = NCRYPT_MACHINE_KEY_FLAG;

            // Cert validity - kinda pointless for a selfsigned cert
            GetSystemTime(&startTime);
            SystemTimeToFileTime(&startTime, &fileTime);
            timeData.LowPart = fileTime.dwLowDateTime;
            timeData.HighPart = (LONG)fileTime.dwHighDateTime;
            timeData.QuadPart += (UINT64)(60 * 60 * 24 * 365 * 10) * (UINT64)10000000; // 10Yr validity
            fileTime.dwLowDateTime = timeData.LowPart;
            fileTime.dwHighDateTime = (DWORD)timeData.HighPart;
            FileTimeToSystemTime(&fileTime, &endTime);

            // Create the self signed certificate
#pragma prefast(push)
#pragma prefast(disable: 33088, "This self signed certificate is used not for security or trust purposes, but as a vessel to carry the public key")
            AutoCloseCertificateContext idkCert(CertCreateSelfSignCertificate(hKey.Get(), &nameBlob, 0, &keyProvInfo, NULL, &startTime, &endTime, &certExtensions));
            AutoCloseHCertStore myStore(CertOpenStore(CERT_STORE_PROV_SYSTEM, PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, NULL, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, L"My"));
            PCCERT_CONTEXT rawIdkCertInStore = nullptr;
            if ((idkCert.get() == NULL) ||
                (myStore.get() == NULL) ||
                (!CertAddCertificateContextToStore(myStore.get(), idkCert.get(), CERT_STORE_ADD_USE_EXISTING, &rawIdkCertInStore)))
            {
                throw TPM_RC_FAILURE;
            }

            AutoCloseCertificateContext idkCertInStore(rawIdkCertInStore);
            if ((status = NCryptSetProperty(hKey.Get(), NCRYPT_CERTIFICATE_PROPERTY, idkCertInStore.get()->pbCertEncoded, idkCertInStore.get()->cbCertEncoded, 0)) != ERROR_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }
#pragma prefast(pop)
        }
        else
        {
            // The key exists and we have opened it
            // Lets hijack the TBS context and TPM key handle from NCrypt
            if (((status = NCryptGetProperty(hAlg.Get(), NCRYPT_PCP_PLATFORMHANDLE_PROPERTY, (PBYTE)&NCryptTbsHandle, sizeof(NCryptTbsHandle), &returnedSize, 0)) != ERROR_SUCCESS) ||
                ((status = NCryptGetProperty(hKey.Get(), NCRYPT_PCP_PLATFORMHANDLE_PROPERTY, (PBYTE)&NCryptTpmKeyHandle, sizeof(NCryptTpmKeyHandle), &returnedSize, 0)) != ERROR_SUCCESS))
            {
                throw TPM_RC_FAILURE;
            }

            // Switch Urchin to the PCPKSP owned TBS context and read the the IDKPub
            g_hTbs = NCryptTbsHandle;
            result = LimpetGetTpmKey2BPub(NCryptTpmKeyHandle, pbIdKeyPub, cbIdKeyPub, pcbIdKeyPub, idKeyName);
        }

        // Retrieve the idkCert from the key handle
        if ((status = NCryptGetProperty(hKey.Get(), NCRYPT_CERTIFICATE_PROPERTY, pbCert, cbCert, (DWORD*)pcbCert, 0)) != ERROR_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }

        // Calculate the cert SHA1 thumbprint
        DWORD cbCertThumbprint = SHA1_DIGEST_SIZE;
        if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM,
            0,
            NULL,
            pbCert,
            *pcbCert,
            certThumbPrint,
            &cbCertThumbprint))
        {
            throw TPM_RC_FAILURE;
        }

        if ((wcName != nullptr) &&
            (StringCchCopyW(wcName, LIMPET_STRING_SIZE, &keyName[0]) != S_OK))
        {
            throw TPM_RC_FAILURE;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    // Cleanup
    g_hTbs = LimpetTbsHandle; // Restore the TBS handle in Urchin
    return result;
}

UINT32 LimpetDestroyIdentityCert(
    __in UINT32 logicalDeviceNumber,
    __in_z LPCWSTR wcAlgorithm,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    NTSTATUS status = ERROR_SUCCESS;
    UINT32 result = TPM_RC_SUCCESS;
    DWORD cbCert = 0;
    CERT_PUBLIC_KEY_INFO pubKey = { 0 };

    try
    {
        // Create the key name for the KSP
        std::wstring keyName(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetReadDeviceId(logicalDeviceNumber, &keyName[0])) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        keyName.resize(keyName.find(L'\0'));
        keyName += std::wstring(wcAlgorithm);
        keyName.push_back(L'\0');

        // Retrieve the idkCert from the key
        AutoCloseNcryptProvHandle hAlg;
        if (((status = NCryptOpenStorageProvider(&hAlg, MS_PLATFORM_KEY_STORAGE_PROVIDER, 0)) != ERROR_SUCCESS) ||
            (hAlg.Get() == NULL))
        {
            throw TPM_RC_FAILURE;
        }
        AutoCloseNcryptKeyHandle hKey;
        if (((status = NCryptOpenKey(hAlg.Get(), &hKey, &keyName[0], 0, NCRYPT_MACHINE_KEY_FLAG)) != ERROR_SUCCESS) ||
            (hKey.Get() == NULL) ||
            ((status = NCryptGetProperty(hKey.Get(), NCRYPT_CERTIFICATE_PROPERTY, NULL, 0, &cbCert, 0)) != ERROR_SUCCESS))
        {
            throw TPM_RC_FAILURE;
        }
        std::vector<BYTE> cert(cbCert);
        if ((status = NCryptGetProperty(hKey.Get(), NCRYPT_CERTIFICATE_PROPERTY, &cert[0], (DWORD)cert.size(), &cbCert, 0)) != ERROR_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }

        // Open the store so we can look for certs with that same public key
        AutoCloseCertificateContext idkCert(CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, &cert[0], cbCert));
        AutoCloseHCertStore myStore(CertOpenStore(CERT_STORE_PROV_SYSTEM, PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, NULL, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, L"My"));
        if ((idkCert.get() == NULL) ||
            (myStore.get() == NULL))
        {
            throw TPM_RC_FAILURE;
        }

        // Delete all certs with that public key from the store
        AutoCloseCertificateContext findCert(CertFindCertificateInStore(myStore.get(), X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_PUBLIC_KEY, &idkCert.get()->pCertInfo->SubjectPublicKeyInfo, nullptr));
        while (findCert.get() != NULL)
        {
            if (!CertDeleteCertificateFromStore(findCert.get()))
            {
                throw TPM_RC_FAILURE;
            }
            findCert.reset(CertFindCertificateInStore(myStore.get(), X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_PUBLIC_KEY, &idkCert.get()->pCertInfo->SubjectPublicKeyInfo, nullptr));
        }

        // Finally delete the key from the machine
        if ((status = NCryptDeleteKey(hKey.Get(), 0)) != ERROR_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }

        if ((wcName != nullptr) &&
            (StringCchCopyW(wcName, LIMPET_STRING_SIZE, &keyName[0]) != S_OK))
        {
            throw TPM_RC_FAILURE;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetImportIdentityCert(
    __in_bcount(cbNewCert) PBYTE pbNewCert,
    __in UINT32 cbNewCert,
    __out_bcount(SHA1_DIGEST_SIZE) PBYTE certThumbPrint,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
)
{
    UINT32 result = TPM_RC_SUCCESS;

    try
    {
        // Get the cert context for that cert
        AutoCloseCertificateContext newIdkCert(CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, pbNewCert, cbNewCert));
        AutoCloseHCertStore myStore(CertOpenStore(CERT_STORE_PROV_SYSTEM, PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, NULL, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, L"My"));
        if ((newIdkCert.get() == NULL) ||
            (myStore.get() == NULL))
        {
            throw TPM_RC_FAILURE;
        }
        AutoCloseCertificateContext idkCert(CertFindCertificateInStore(myStore.get(), X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_PUBLIC_KEY, &newIdkCert.get()->pCertInfo->SubjectPublicKeyInfo, nullptr));
        if (idkCert.get() == NULL)
        {
            throw TPM_RC_FAILURE;
        }

        // Obtain the private key handle in the provider
        DWORD dwKeySpec = 0;
        BOOL fCallerFreeProvOrNCryptKey = FALSE;
        AutoCloseNcryptKeyHandle hKey;
        if (!CryptAcquireCertificatePrivateKey(idkCert.get(), CRYPT_ACQUIRE_ONLY_NCRYPT_KEY_FLAG, NULL, &hKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey))
        {
            throw TPM_RC_FAILURE;
        }

        // Copy the key info from old cert to the new and insert the cert into the store and set it on the key container
        DWORD cbKeyProvInfo = sizeof(CRYPT_KEY_PROV_INFO);
        if (!CertGetCertificateContextProperty(idkCert.get(), CERT_KEY_PROV_INFO_PROP_ID, NULL, &cbKeyProvInfo))
        {
            throw TPM_RC_FAILURE;
        }
        std::vector<BYTE> keyProvInfo(cbKeyProvInfo);
        PCCERT_CONTEXT rawIdkCertInStore = nullptr;
        if ((!CertGetCertificateContextProperty(idkCert.get(), CERT_KEY_PROV_INFO_PROP_ID, &keyProvInfo[0], &cbKeyProvInfo)) ||
            (!CertSetCertificateContextProperty(newIdkCert.get(), CERT_KEY_PROV_INFO_PROP_ID, 0, &keyProvInfo[0])) ||
            (!CertAddCertificateContextToStore(myStore.get(), newIdkCert.get(), CERT_STORE_ADD_ALWAYS, &rawIdkCertInStore)))
        {
            throw TPM_RC_FAILURE;
        }
        AutoCloseCertificateContext idkCertInStore(rawIdkCertInStore);
        if ((idkCertInStore.get() == NULL) ||
            (NCryptSetProperty(hKey.Get(), NCRYPT_CERTIFICATE_PROPERTY, idkCertInStore.get()->pbCertEncoded, idkCertInStore.get()->cbCertEncoded, 0) != ERROR_SUCCESS))
        {
            throw TPM_RC_FAILURE;
        }

        // Calculate the new certs thumbprint
        DWORD cbCertThumbprint = SHA1_DIGEST_SIZE;
        if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM,
            0,
            NULL,
            idkCertInStore.get()->pbCertEncoded,
            idkCertInStore.get()->cbCertEncoded,
            certThumbPrint,
            &cbCertThumbprint))
        {
            throw TPM_RC_FAILURE;
        }

        // Return the KSP key container name
        PCRYPT_KEY_PROV_INFO pKeyProvInfo = (PCRYPT_KEY_PROV_INFO)&keyProvInfo[0];
        if ((wcName != nullptr) &&
            (StringCchCopyW(wcName, LIMPET_STRING_SIZE, pKeyProvInfo->pwszContainerName) != S_OK))
        {
            throw TPM_RC_FAILURE;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetSignWithIdentityCert(
    __in_bcount(SHA1_DIGEST_SIZE) PBYTE certThumbPrint,
    __in_bcount(cbDataToSign) PBYTE pbDataToSign,
    __in UINT32 cbDataToSign,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* subjectName,
    __out_bcount_part(cbSignature, *pcbSignature) PBYTE pbSignature,
    __in UINT32 cbSignature,
    __out PUINT32 pcbSignature
)
{
    NTSTATUS status = ERROR_SUCCESS;
    UINT32 result = TPM_RC_SUCCESS;
    CRYPT_HASH_BLOB thumbPrint = { SHA1_DIGEST_SIZE, certThumbPrint };

    try
    {
        // Calculate the SHA256 digest over the data
        std::vector<BYTE> dataDigest(SHA256_DIGEST_SIZE);
        DWORD cbDigest = (DWORD)dataDigest.size();
        if (!CryptHashCertificate2(BCRYPT_SHA256_ALGORITHM,
            0,
            NULL,
            pbDataToSign,
            cbDataToSign,
            &dataDigest[0],
            &cbDigest))
        {
            throw TPM_RC_FAILURE;
        }

        // Open the certificate that matches that thumbprint and get a private key handle for the key in the TPM
        AutoCloseHCertStore myStore(CertOpenStore(CERT_STORE_PROV_SYSTEM, PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, NULL, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, L"My"));
        if (myStore.get() == NULL)
        {
            throw TPM_RC_FAILURE;
        }

        AutoCloseCertificateContext idkCert(CertFindCertificateInStore(myStore.get(), X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_HASH, &thumbPrint, nullptr));
        AutoCloseNcryptKeyHandle hKey;
        DWORD dwKeySpec = 0;
        BOOL fCallerFreeProvOrNCryptKey = FALSE;
        if ((idkCert.get() == NULL) ||
            (CertNameToStrW(X509_ASN_ENCODING, &idkCert.get()->pCertInfo->Subject, CERT_X500_NAME_STR, subjectName, LIMPET_STRING_SIZE) <= 1) ||
            (!CryptAcquireCertificatePrivateKey(idkCert.get(), CRYPT_ACQUIRE_ONLY_NCRYPT_KEY_FLAG, NULL, &hKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey)))
        {
            throw TPM_RC_FAILURE;
        }

        std::wstring algorithmId(LIMPET_STRING_SIZE, L'\0');
        DWORD cbAlgorithmId = (DWORD)algorithmId.capacity() * sizeof(WCHAR);
        if (NCryptGetProperty(hKey.Get(), NCRYPT_ALGORITHM_PROPERTY, (PBYTE)&algorithmId[0], cbAlgorithmId, &cbAlgorithmId, 0) != ERROR_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }
        algorithmId.resize(algorithmId.find(L'\0'));

        // Create the signature
        if (algorithmId.find(BCRYPT_RSA_ALGORITHM) != std::wstring::npos)
        {
            BCRYPT_PKCS1_PADDING_INFO padding = { NCRYPT_SHA256_ALGORITHM };

            if ((status = NCryptSignHash(hKey.Get(),
                &padding,
                &dataDigest[0],
                (DWORD)dataDigest.size(),
                pbSignature,
                cbSignature,
                (PDWORD)pcbSignature,
                BCRYPT_PAD_PKCS1)) != ERROR_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }
        }
        // Bug 8678102: The provider returns NCRYPT_ECDSA_ALGORITHM_GROUP instesad of BCRYPT_ECDSA_P256_ALGORITHM so we only look at the first 5 characters
        else if (algorithmId.find(BCRYPT_ECDSA_P256_ALGORITHM, 0, 5) != std::wstring::npos)
        {
            if ((status = NCryptSignHash(hKey.Get(),
                NULL,
                &dataDigest[0],
                (DWORD)dataDigest.size(),
                pbSignature,
                cbSignature,
                (PDWORD)pcbSignature,
                0)) != ERROR_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }
        }
        else
        {
            throw TPM_RC_FAILURE;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

LSTATUS ReadHostageBlob(const UINT32 logicalDeviceNumber, PBYTE hostageBlob, DWORD *cbhostageBlob)
{
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    ULONG nError;
    HKEY hKeyNtservice;

    nError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, logicalDeviceNumberPath.c_str(), 0, KEY_READ, &hKeyNtservice);

    // return error, if key does not exist.
    if (ERROR_BADKEY == nError || ERROR_FILE_NOT_FOUND == nError)
    {
        *cbhostageBlob = 0;
        return nError;
    }
    else if (ERROR_SUCCESS != nError)
    {
        *cbhostageBlob = 0;
        return nError;
    }

    DWORD cbData = 0;
    DWORD dwRegType = REG_BINARY;
    nError = RegQueryValueEx(hKeyNtservice, REG_KEY_NAME_HOSTAGEBLOB.c_str(), NULL, &dwRegType, NULL, &cbData);
    if (ERROR_BADKEY == nError || ERROR_FILE_NOT_FOUND == nError)
    {
        *cbhostageBlob = 0;
        return nError;
    }
    else if (ERROR_SUCCESS != nError)
    {
        *cbhostageBlob = 0;
        return nError;
    }

    if (*cbhostageBlob < cbData)
    {
        return E_NOT_SUFFICIENT_BUFFER;
    }

    RegQueryValueEx(hKeyNtservice, REG_KEY_NAME_HOSTAGEBLOB.c_str(), NULL, &dwRegType, hostageBlob, cbhostageBlob);

    if (hKeyNtservice != nullptr)
    {
        RegCloseKey(hKeyNtservice);
        hKeyNtservice = nullptr;
    }

    return S_OK;
}

#pragma prefast(suppress: __WARNING_EXCESSIVESTACKUSAGE, "WDGVSO:12767047 Code follows TCG/ISO/IEC standards.")
UINT32 LimpetSignWithIdentityHmacKeyEx(
    __in UINT32 logicalDeviceNumber,
    __in_bcount(cbDataToSign) PCBYTE pbDataToSign,
    __in UINT32 cbDataToSign,
    __out_bcount(SHA256_DIGEST_SIZE) PBYTE pbSignature
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        Load_In load;
        HMAC_In hmac;
        HMAC_Start_In hmac_Start;
        SequenceUpdate_In sequenceUpdate;
        SequenceComplete_In sequenceComplete;
        FlushContext_In flushContext;
    } tpm_in;
    union
    {
        Load_Out load;
        HMAC_Out hmac;
        HMAC_Start_Out hmac_Start;
        SequenceUpdate_Out sequenceUpdate;
        SequenceComplete_Out sequenceComplete;
        FlushContext_Out flushContext;
    } tpm_out;

    try
    {
        std::unique_ptr<ANY_OBJECT> hmacKey(new ANY_OBJECT);

        if (UseRegistry(logicalDeviceNumber))
        {
            // read the hmac public key from registry.
            const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);

            // define buffer for hmac
            BYTE hmacBuffer[2048];
            INT32 cbhmacBuffer = 2048;
            for (INT32 i = 0; i < cbhmacBuffer; i++)
            {
                hmacBuffer[i] = '\0';
            }
            if ((result = ReadRegistryBinaryData(logicalDeviceNumberPath, REG_KEY_NAME_HMACPUBLICKEYNAME, (BYTE*)hmacBuffer, &cbhmacBuffer)) != 0)
            {
                throw result;
            }

            // unmarshal hmac public key buffer to TPM2B_PUBLIC object.
            buffer = hmacBuffer;
            size = cbhmacBuffer;
            if ((result = TPM2B_PUBLIC_Unmarshal(&hmacKey->obj.publicArea, &buffer, &size, TRUE)) != TPM_RC_SUCCESS)
            {
                throw result;
            }

            // read the hmac private key from registry.
            cbhmacBuffer = 2048;
            for (INT32 i = 0; i < cbhmacBuffer; i++)
            {
                hmacBuffer[i] = '\0';
            }

            if ((result = ReadRegistryBinaryData(logicalDeviceNumberPath, REG_KEY_NAME_HMACPRIVATEKEYNAME, (BYTE*)hmacBuffer, &cbhmacBuffer)) != 0)
            {
                throw result;
            }

            // unmarshal hmac private key buffer to TPM2B_PRIVATE object.
            buffer = hmacBuffer;
            size = cbhmacBuffer;
            if ((result = TPM2B_PRIVATE_Unmarshal(&hmacKey->obj.privateArea, &buffer, &size)) != TPM_RC_SUCCESS)
            {
                throw result;
            }

            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_Load, &tpm_in.load, &tpm_out.load);
            parms.objectTableIn[TPM2_Load_HdlIn_ParentHandle].generic.handle = LIMPET_TPM20_SRK_HANDLE;
            tpm_in.load.inPrivate = hmacKey->obj.privateArea;
            tpm_in.load.inPublic = hmacKey->obj.publicArea;
            LIMPET_TRY_TPM_CALL(FALSE, TPM2_Load);

            // For any reason if Tpm failed to load the public/private key, deleting the keys from registry.
            if (FAILED_WIN32(result))
            {
                LimpetEvictHmacKey(logicalDeviceNumber, nullptr);
                throw result;
            }

            *hmacKey = parms.objectTableOut[TPM2_Load_HdlOut_ObjectHandle];
        }

        // The TPM provides a simple one command HMAC operation if the data is at or smaller than 1k
        if (cbDataToSign <= 1024)
        {
            // Feed it through the grinder
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_HMAC, &tpm_in.hmac, &tpm_out.hmac);
            if (UseRegistry(logicalDeviceNumber))
            {
                parms.objectTableIn[TPM2_HMAC_HdlIn_Handle] = *hmacKey;
            }
            else
            {
                parms.objectTableIn[TPM2_HMAC_HdlIn_Handle].obj.handle = LIMPET_TPM20_PERSISTED_KEY_INDEX + logicalDeviceNumber;
            }
            tpm_in.hmac.hashAlg = TPM_ALG_SHA256;
            MemoryCopy(tpm_in.hmac.buffer.t.buffer, pbDataToSign, cbDataToSign, sizeof(tpm_in.hmac.buffer.t.buffer));
            tpm_in.hmac.buffer.t.size = (UINT16)cbDataToSign; // cbDataToSign is less than 1024 (checked above) so this cast is safe
            LIMPET_TPM_CALL(FALSE, TPM2_HMAC);

            // Return the HMAC
            MemoryCopy(pbSignature, tpm_out.hmac.outHMAC.t.buffer, tpm_out.hmac.outHMAC.t.size, 32);
        }
        else
        {
            ANY_OBJECT sequence = { 0 };
            UINT32 dataIndex = 0;

            // Start SHA-256 HMAC
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_HMAC_Start, &tpm_in.hmac_Start, &tpm_out.hmac_Start);
            if (UseRegistry(logicalDeviceNumber))
            {
                parms.objectTableIn[TPM2_HMAC_Start_HdlIn_Handle] = *hmacKey;
            }
            else
            {
                parms.objectTableIn[TPM2_HMAC_Start_HdlIn_Handle].obj.handle = LIMPET_TPM20_PERSISTED_KEY_INDEX + logicalDeviceNumber;
            }
            tpm_in.hmac_Start.hashAlg = TPM_ALG_SHA256;
            LIMPET_TPM_CALL(FALSE, TPM2_HMAC_Start);
            sequence = parms.objectTableOut[TPM2_HashSequenceStart_HdlOut_SequenceHandle];

            // Iterate through the file until we have only 1024 or less left
            while ((cbDataToSign - dataIndex) > 1024)
            {
                // Update the SHA-256 digest
                INITIALIZE_CALL_BUFFERS(TPM2_SequenceUpdate, &tpm_in.sequenceUpdate, &tpm_out.sequenceUpdate);
                parms.objectTableIn[TPM2_SequenceUpdate_HdlIn_SequenceHandle] = sequence;
                tpm_in.sequenceUpdate.buffer.t.size = 1024;
                MemoryCopy(tpm_in.sequenceUpdate.buffer.t.buffer, &pbDataToSign[dataIndex], tpm_in.sequenceUpdate.buffer.t.size, sizeof(tpm_in.sequenceUpdate.buffer.t.buffer));
                LIMPET_TPM_CALL(FALSE, TPM2_SequenceUpdate);
                dataIndex += tpm_in.sequenceUpdate.buffer.t.size;
            }

            // Finalize with the last data and get the SHA256 HMAC
            INITIALIZE_CALL_BUFFERS(TPM2_SequenceComplete, &tpm_in.sequenceComplete, &tpm_out.sequenceComplete);
            parms.objectTableIn[TPM2_SequenceUpdate_HdlIn_SequenceHandle] = sequence;
            tpm_in.sequenceComplete.hierarchy = TPM_RH_NULL;
            tpm_in.sequenceComplete.buffer.t.size = (UINT16)(cbDataToSign - dataIndex); // This is the last round so the data size is <1024 and that fits into UINT16
            MemoryCopy(tpm_in.sequenceComplete.buffer.t.buffer, &pbDataToSign[dataIndex], tpm_in.sequenceComplete.buffer.t.size, sizeof(tpm_in.sequenceComplete.buffer.t.buffer));
            LIMPET_TPM_CALL(FALSE, TPM2_SequenceComplete);
            sequence = parms.objectTableIn[TPM2_SequenceComplete_HdlIn_SequenceHandle];

            // Return the HMAC
            MemoryCopy(pbSignature, tpm_out.sequenceComplete.result.t.buffer, tpm_out.sequenceComplete.result.t.size, 32);
        }

        if (UseRegistry(logicalDeviceNumber))
        {
            // Unload the hmac key from TPM.
            INITIALIZE_CALL_BUFFERS(TPM2_FlushContext, &tpm_in.flushContext, &tpm_out.flushContext);
            parms.objectTableIn[TPM2_FlushContext_HdlIn_FlushHandle] = *hmacKey;
            LIMPET_TPM_CALL(FALSE, TPM2_FlushContext);
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}


UINT32 ImportHostageBlobIfPresent(__in UINT32 logicalDeviceNumber)
{
    INT32 cbhostageBlob = 0;
    UINT32 result = TPM_RC_SUCCESS;

    if (UseRegistry(logicalDeviceNumber) &&
        IsLazyLoadingEnabled() &&
        IsHostageBlobPresent(logicalDeviceNumber, &cbhostageBlob))
    {
        std::vector<BYTE> hostageBlob(cbhostageBlob, '\0');
        std::vector<BYTE> activationSecret(MAX_AES_KEY_BYTES);
        const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);

        try
        {
            result = ReadRegistryBinaryData(logicalDeviceNumberPath, REG_KEY_NAME_HOSTAGEBLOB, &hostageBlob[0], &cbhostageBlob);

            if (SUCCEEDED_WIN32(result))
            {
                result = LimpetImportHostageKeyEx(logicalDeviceNumber,
                    &hostageBlob[0],
                    (UINT32)cbhostageBlob,
                    &activationSecret[0],
                    nullptr,
                    nullptr);
            }
        }
        catch (UINT32 err)
        {
            result = err;
        }
        catch (...)
        {
            result = TPM_RC_FAILURE;
        }

        // If Tpm failed to load the blob throw error and delete the blob.
        // If Tpm successfully decrypted the blob, we no longer needs the blob.
        DeleteHostageBlob(logicalDeviceNumber);
    }

    return result;
}

UINT32 LimpetSignWithIdentityHmacKey(
    __in UINT32 logicalDeviceNumber,
    __in_bcount(cbDataToSign) PCBYTE pbDataToSign,
    __in UINT32 cbDataToSign,
    __out_bcount(SHA256_DIGEST_SIZE) PBYTE pbSignature
)
{
    UINT32 result = TPM_RC_SUCCESS;

    if (!SUCCEEDED_WIN32(result = ImportHostageBlobIfPresent(logicalDeviceNumber)))
    {
        return result;
    }
    else
    {
        return LimpetSignWithIdentityHmacKeyEx(
            logicalDeviceNumber,
            pbDataToSign,
            cbDataToSign,
            pbSignature
        );
    }
}

UINT32 LimpetGenerateSASToken(
    __in UINT32 logicalDeviceNumber,
    __in LARGE_INTEGER* expiration,
    __out_ecount_z(*pcchConnectionString) WCHAR* pszConnectionString,
    _Inout_ size_t *pcchConnectionString
)
{
    UINT32 result = TPM_RC_SUCCESS;

    if (pcchConnectionString == nullptr)
    {
        result = static_cast<UINT32>(E_INVALIDARG);
        return result;
    }
    try
    {
        if (!SUCCEEDED_WIN32(result = ImportHostageBlobIfPresent(logicalDeviceNumber)))
        {
            return result;
        }

        // Take the URI apart and turn the components into wide character strings
        std::string uriData(LIMPET_STRING_SIZE, '\0');
        UINT32 cbUriData;
        if ((result = LimpetReadURI(logicalDeviceNumber, (PBYTE)&uriData[0], (UINT32)uriData.capacity(), &cbUriData)) != TPM_RC_SUCCESS)
        {
            return result;
        }
        uriData.resize(cbUriData);

        // Do we have an explicit device name in the string?
        size_t device_id_separator;
        if ((device_id_separator = uriData.find('/')) == std::string::npos)
        {
            device_id_separator = (size_t)-1;
        }
        else
        {
            uriData[device_id_separator] = '\0';
        }

        size_t module_id_separator = (size_t)-1;
        if (device_id_separator != (size_t)-1)
        {
            // Do we have an explicit module name in the string?
            if ((module_id_separator = uriData.find('/', device_id_separator + 1)) == std::string::npos)
            {
                module_id_separator = (size_t)-1;
            }
            else
            {
                uriData[module_id_separator] = '\0';
            }
        }

        // Create URI ... assume this is a Device (host/devices/device)
        std::wstring uri(LIMPET_STRING_SIZE, '\0');
        size_t cchUri;
        if (((FAILED(StringCchPrintfW(&uri[0],
            uri.capacity(),
            L"%S/devices/%S",
            &uriData[0],
            &uriData[device_id_separator + 1]))) ||
            (FAILED(StringCchLengthW(&uri[0], uri.capacity(), &cchUri)))))
        {
            throw TPM_RC_FAILURE;
        }
        // If this is a module, apend module info (/modules/module)
        if (module_id_separator != -1)
        {
            if (((FAILED(StringCchPrintfW(&uri[cchUri],
                uri.capacity() - cchUri,
                L"/modules/%S",
                &uriData[module_id_separator + 1])))) ||
                (FAILED(StringCchLengthW(&uri[0], uri.capacity(), &cchUri))))
            {
                throw TPM_RC_FAILURE;
            }
        }
        uri.resize(cchUri);

        // Prepare the message to be signed
        std::string message(LIMPET_STRING_SIZE, '\0');
        size_t cchMessage;
        if (((FAILED(StringCchPrintfA(&message[0],
            message.capacity(),
            "%S\n%lld",
            &uri[0],
            expiration->QuadPart))) ||
            (FAILED(StringCchLengthA(&message[0], message.capacity(), &cchMessage)))))
        {
            throw TPM_RC_FAILURE;
        }
        message.resize(cchMessage);

        // Get the message HMAC signed
        std::vector<BYTE> hmac(SHA256_DIGEST_SIZE);
        if ((result = LimpetSignWithIdentityHmacKey(logicalDeviceNumber, (PBYTE)&message[0], (UINT32)message.length(), &hmac[0])) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        std::wstring printableHmac(LIMPET_STRING_SIZE, L'\0');
        DWORD cchPrintableHmac = (DWORD)printableHmac.capacity();
        if (!CryptBinaryToStringW(&hmac[0], (DWORD)hmac.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &printableHmac[0], &cchPrintableHmac))
        {
            throw TPM_RC_FAILURE;
        }
        hmac.resize(cchPrintableHmac);
        std::wstring printableHmacClean(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetSanitizeString(&printableHmac[0], &printableHmacClean[0])) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        printableHmacClean.resize(printableHmacClean.find(L'\0'));

        // Create token context ... assume this is a Device (HostName=XXX;DeviceId=XXX)
        std::wstring context(LIMPET_STRING_SIZE, '\0');
        size_t cchContext;
        if (((FAILED(StringCchPrintfW(&context[0],
            context.capacity(),
            L"HostName=%S;DeviceId=%S",
            &uriData[0],
            &uriData[device_id_separator + 1]))) ||
            (FAILED(StringCchLengthW(&context[0], context.capacity(), &cchContext)))))
        {
            throw TPM_RC_FAILURE;
        }
        // If this is a module, apend module info (;ModuleId=XXX)
        if (module_id_separator != -1)
        {
            if (((FAILED(StringCchPrintfW(&context[cchContext],
                context.capacity() - cchContext,
                L";ModuleId=%S",
                &uriData[module_id_separator + 1])))) ||
                (FAILED(StringCchLengthW(&context[0], context.capacity(), &cchContext))))
            {
                throw TPM_RC_FAILURE;
            }
        }
        context.resize(cchContext);

        // Put the connection string together
        return StringCchPrintfW(pszConnectionString,
            *pcchConnectionString,
            L"%s;SharedAccessSignature=SharedAccessSignature sr=%s&sig=%s&se=%lld",
            &context[0],
            &uri[0],
            &printableHmacClean[0],
            expiration->QuadPart);
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetIssueCertificate(
    __in_bcount(SHA1_DIGEST_SIZE) PBYTE certThumbPrint,
    __in_bcount(cbCertReq) PBYTE pbCertReq,
    UINT32 cbCertReq,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* deviceName,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* url,
    __out_bcount_opt(SHA256_DIGEST_SIZE + sizeof(UINT16)) PBYTE tpmKeyName,
    __out_bcount(SHA1_DIGEST_SIZE) PBYTE newCertThumbPrint,
    __out_bcount_part(cbOutput, *pcbResult) PBYTE pbOutput,
    UINT32 cbOutput,
    __out PUINT32 pcbResult
)
{
    UINT32 result = TPM_RC_SUCCESS;
    CRYPT_HASH_BLOB thumbPrint = { SHA1_DIGEST_SIZE, certThumbPrint };
    PCRYPT_DATA_BLOB  pTpmPublic = NULL;
    DWORD cbTpmPublic = 0;
    PBYTE buffer = NULL;
    INT32 size = 0;
    std::vector<BYTE> keyIdExtension(1024);
    TPM2B_PUBLIC certKey = { 0 };
    CERT_INFO certInfo = { 0 };
    CERT_BASIC_CONSTRAINTS2_INFO basicConstraintsInfo = { FALSE, // fCA
                                                          FALSE, // fPathLenConstraint
                                                          0 };   // dwPathLenConstraint
    SYSTEMTIME systemTime = { 0 };
    CERT_EXTENSION certExtensions[] = { { const_cast<char*>(szOID_SUBJECT_KEY_IDENTIFIER), FALSE,{ 0, NULL } },
                                        { const_cast<char*>(szOID_BASIC_CONSTRAINTS2), TRUE, { 0, NULL } },
                                        { const_cast<char*>(szOID_AUTHORITY_KEY_IDENTIFIER2), FALSE, { 0, NULL } } };
    PBYTE pbEncAuthorityKeyInfo = NULL;
    DWORD cbEncAuthorityKeyInfo = 0;
    std::vector<BYTE> authorityKeyId(SHA1_DIGEST_SIZE);
    CERT_AUTHORITY_KEY_ID2_INFO keyIdInfo = { 0 };
    keyIdInfo.KeyId.cbData = (UINT32)authorityKeyId.size();
    keyIdInfo.KeyId.pbData = &authorityKeyId[0];
    std::string oidRsaSha256SA{ szOID_RSA_SHA256RSA };
    CRYPT_ALGORITHM_IDENTIFIER certAlgId = { &oidRsaSha256SA[0], { 0, NULL } };

    try
    {
        // Find the CA Cert and get the private key handle
        AutoCloseHCertStore myStore(CertOpenStore(CERT_STORE_PROV_SYSTEM, PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, NULL, CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG, L"My"));
        if (myStore.get() == NULL)
        {
            throw TPM_RC_FAILURE;
        }
        AutoCloseCertificateContext caCert(CertFindCertificateInStore(myStore.get(), X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_HASH, &thumbPrint, nullptr));
        if (caCert.get() == NULL)
        {
            throw TPM_RC_FAILURE;
        }
        AutoCloseNcryptKeyHandle hKey;
        DWORD dwKeySpec = 0;
        BOOL fCallerFreeProvOrNCryptKey = FALSE;
        if ((!CryptAcquireCertificatePrivateKey(caCert.get(), CRYPT_ACQUIRE_ONLY_NCRYPT_KEY_FLAG, NULL, &hKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey)) ||
            (hKey.Get() == NULL))
        {
            throw TPM_RC_FAILURE;
        }

        // Open the self signed cert from the client so we can copy things out
        AutoCloseCertificateContext selfSignedCert(CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, pbCertReq, cbCertReq));
        if (selfSignedCert.get() == NULL)
        {
            throw TPM_RC_FAILURE;
        }

        // Iterate through the extensions and find the TPM public key in the cert request, not having that is fatal.
        for (UINT32 n = 0; n < selfSignedCert.get()->pCertInfo->cExtension; n++)
        {
            if ((!strcmp(selfSignedCert.get()->pCertInfo->rgExtension[n].pszObjId, szOID_SUBJECT_KEY_IDENTIFIER)) &&
                (selfSignedCert.get()->pCertInfo->rgExtension[0].fCritical == FALSE) &&
                (CryptDecodeObjectEx(X509_ASN_ENCODING,
                    szOID_SUBJECT_KEY_IDENTIFIER,
                    selfSignedCert.get()->pCertInfo->rgExtension[0].Value.pbData,
                    selfSignedCert.get()->pCertInfo->rgExtension[0].Value.cbData,
                    CRYPT_DECODE_ALLOC_FLAG,
                    NULL,
                    &pTpmPublic,
                    &cbTpmPublic)))
            {
                // Copy the TPM key extension to the cert to be issued
                certExtensions[0] = selfSignedCert.get()->pCertInfo->rgExtension[n];
                break;
            }
        }
        if ((pTpmPublic == NULL) || (cbTpmPublic == 0))
        {
            throw TPM_RC_FAILURE;
        }

        // Re-hydrate the TPM public key and compare it to the public key from the cert
        buffer = (PBYTE)pTpmPublic->pbData;
        size = (INT32)pTpmPublic->cbData;
        if (TPM2B_PUBLIC_Unmarshal(&certKey, &buffer, &size, FALSE) != TPM_RC_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }

        // Re-hydrate the certificate public key
        BCRYPT_KEY_HANDLE rawCertKey = nullptr;
        DWORD cbProperty;
        if (!CryptImportPublicKeyInfoEx2(X509_ASN_ENCODING, &selfSignedCert.get()->pCertInfo->SubjectPublicKeyInfo, 0, NULL, &rawCertKey))
        {
            throw TPM_RC_FAILURE;
        }
        AutoCloseBcryptKeyHandle hCertKey(rawCertKey);
        if ((BCryptGetProperty(hCertKey.get(), BCRYPT_ALGORITHM_NAME, NULL, 0, &cbProperty, 0) != ERROR_SUCCESS))
        {
            throw TPM_RC_FAILURE;
        }
        std::vector<BYTE> property(cbProperty);
        if (BCryptGetProperty(hCertKey.get(), BCRYPT_ALGORITHM_NAME, &property[0], (DWORD)property.size(), &cbProperty, 0) != ERROR_SUCCESS)
        {
            throw TPM_RC_FAILURE;
        }

        // Verify that the public keys match
        if (!wcscmp((WCHAR*)&property[0], BCRYPT_RSA_ALGORITHM))
        {
            // RSA public key verification
            if (BCryptExportKey(hCertKey.get(), NULL, BCRYPT_RSAPUBLIC_BLOB, NULL, 0, &cbProperty, 0) != ERROR_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }
            std::vector<BYTE> pubkey(cbProperty);
            if (BCryptExportKey(hCertKey.get(), NULL, BCRYPT_RSAPUBLIC_BLOB, &pubkey[0], (DWORD)pubkey.size(), &cbProperty, 0) != ERROR_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }
            BCRYPT_RSAKEY_BLOB* keyData = (BCRYPT_RSAKEY_BLOB*)&pubkey[0];
            if ((keyData->Magic != BCRYPT_RSAPUBLIC_MAGIC) ||
                (keyData->cbModulus != certKey.t.publicArea.unique.rsa.t.size) ||
                (memcmp(certKey.t.publicArea.unique.rsa.t.buffer, &pubkey[sizeof(BCRYPT_RSAKEY_BLOB) + keyData->cbPublicExp], keyData->cbModulus)))
            {
                throw TPM_RC_FAILURE;
            }
        }
        else if (!wcscmp((WCHAR*)&property[0], BCRYPT_ECDSA_P256_ALGORITHM))
        {
            // ECC public key verification
            if (BCryptExportKey(hCertKey.get(), NULL, BCRYPT_ECCPUBLIC_BLOB, NULL, 0, &cbProperty, 0) != ERROR_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }
            std::vector<BYTE> pubkey(cbProperty);
            if (BCryptExportKey(hCertKey.get(), NULL, BCRYPT_ECCPUBLIC_BLOB, &pubkey[0], (DWORD)pubkey.size(), &cbProperty, 0) != ERROR_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }
            BCRYPT_ECCKEY_BLOB* keyData = (BCRYPT_ECCKEY_BLOB*)&pubkey[0];
            if ((keyData->dwMagic != BCRYPT_ECDSA_PUBLIC_P256_MAGIC) ||
                ((keyData->cbKey * 2) != (ULONG)(certKey.t.publicArea.unique.ecc.x.t.size + certKey.t.publicArea.unique.ecc.y.t.size)) ||
                (!MemoryEqual(certKey.t.publicArea.unique.ecc.x.t.buffer, &pubkey[sizeof(BCRYPT_ECCKEY_BLOB)], keyData->cbKey)) ||
                (!MemoryEqual(certKey.t.publicArea.unique.ecc.y.t.buffer, &pubkey[sizeof(BCRYPT_ECCKEY_BLOB) + keyData->cbKey], keyData->cbKey)))
            {
                throw TPM_RC_FAILURE;
            }
        }
        else
        {
            throw TPM_RC_FAILURE;
        }

        // Make sure the TPM key properties indicate a randomly generated key that has never and will never see the light of day
        if ((certKey.t.publicArea.objectAttributes.fixedTPM == CLEAR) ||
            (certKey.t.publicArea.objectAttributes.fixedParent == CLEAR) ||
            (certKey.t.publicArea.objectAttributes.sensitiveDataOrigin == CLEAR))
        {
            throw TPM_RC_FAILURE;
        }

        // Calculate the SHA256 TPM key name from the public key for external verification
        if (tpmKeyName != NULL)
        {
            TPM2B_NAME certKeyName = { 0 };
            ObjectComputeName(&certKey.t.publicArea, &certKeyName);
            MemoryCopy(tpmKeyName, certKeyName.t.name, certKeyName.t.size, sizeof(UINT16) + SHA256_DIGEST_SIZE);
        }

        // Put the device certificate together
        certInfo.dwVersion = CERT_V3;
        std::vector<BYTE> certSerial(16);
        certInfo.SerialNumber.cbData = CryptGenerateRandom((UINT16)certSerial.size(), &certSerial[0]);
        certInfo.SerialNumber.pbData = &certSerial[0];
        std::string oidRsaSha256Rsa{ szOID_RSA_SHA256RSA };
        certInfo.SignatureAlgorithm.pszObjId = &oidRsaSha256Rsa[0];
        certInfo.Issuer.cbData = caCert.get()->pCertInfo->Issuer.cbData;
        certInfo.Issuer.pbData = caCert.get()->pCertInfo->Issuer.pbData;
        GetSystemTime(&systemTime);
        SystemTimeToFileTime(&systemTime, &certInfo.NotBefore);
        systemTime.wYear += 10;
        SystemTimeToFileTime(&systemTime, &certInfo.NotAfter);
        certInfo.SubjectPublicKeyInfo = selfSignedCert.get()->pCertInfo->SubjectPublicKeyInfo;

        // Create the subject name
        std::wstring selfSignedSubjectName(LIMPET_STRING_SIZE, L'\0');
        if (!CertNameToStr(X509_ASN_ENCODING, &selfSignedCert.get()->pCertInfo->Subject, CERT_X500_NAME_STR, &selfSignedSubjectName[0], (DWORD)selfSignedSubjectName.capacity()))
        {
            throw TPM_RC_FAILURE;
        }
        selfSignedSubjectName.resize(selfSignedSubjectName.find(L'\0'));
        std::wstring subjectName(LIMPET_STRING_SIZE, L'\0');
        if (FAILED(StringCchPrintf(&subjectName[0], (DWORD)subjectName.capacity(), L"%s, OU=%s, DC=%s", &selfSignedSubjectName[0], deviceName, url)))
        {
            throw TPM_RC_FAILURE;
        }
        subjectName.resize(subjectName.find(L'\0'));
        DWORD cbNameBlobData = 0;
        if (!CertStrToName(X509_ASN_ENCODING, &subjectName[0], CERT_OID_NAME_STR, NULL, NULL, &cbNameBlobData, NULL))
        {
            throw TPM_RC_FAILURE;
        }
        std::vector<BYTE> nameBlobData(cbNameBlobData);
        if (!CertStrToName(X509_ASN_ENCODING, &subjectName[0], CERT_OID_NAME_STR, NULL, &nameBlobData[0], &cbNameBlobData, NULL))
        {
            throw TPM_RC_FAILURE;

        }
        certInfo.Subject.pbData = &nameBlobData[0];
        certInfo.Subject.cbData = (DWORD)nameBlobData.size();

        // Encode the basic constraints extension
        if (!CryptEncodeObjectEx(X509_ASN_ENCODING,
            X509_BASIC_CONSTRAINTS2,
            &basicConstraintsInfo,
            CRYPT_ENCODE_ALLOC_FLAG,
            NULL,
            &certExtensions[1].Value.pbData,
            &certExtensions[1].Value.cbData))
        {
            throw TPM_RC_FAILURE;
        }

        // Encode the authority public key info
        if (!CryptEncodeObjectEx(X509_ASN_ENCODING,
            X509_PUBLIC_KEY_INFO,
            &caCert.get()->pCertInfo->SubjectPublicKeyInfo,
            CRYPT_ENCODE_ALLOC_FLAG,
            NULL,
            &pbEncAuthorityKeyInfo,
            &cbEncAuthorityKeyInfo))
        {
            throw TPM_RC_FAILURE;
        }
        if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM,
            0,
            NULL,
            pbEncAuthorityKeyInfo,
            cbEncAuthorityKeyInfo,
            keyIdInfo.KeyId.pbData,
            &keyIdInfo.KeyId.cbData))
        {
            throw TPM_RC_FAILURE;
        }
        keyIdInfo.AuthorityCertSerialNumber.cbData = caCert.get()->pCertInfo->SerialNumber.cbData;
        keyIdInfo.AuthorityCertSerialNumber.pbData = caCert.get()->pCertInfo->SerialNumber.pbData;
        if (!CryptEncodeObjectEx(X509_ASN_ENCODING,
            X509_AUTHORITY_KEY_ID2,
            &keyIdInfo,
            CRYPT_ENCODE_ALLOC_FLAG,
            NULL,
            &certExtensions[2].Value.pbData,
            &certExtensions[2].Value.cbData))
        {
            throw TPM_RC_FAILURE;
        }

        // Set the extensions
        certInfo.cExtension = 3;
        certInfo.rgExtension = certExtensions;

        // Issue the new certificate
        *pcbResult = cbOutput;
        if (!CryptSignAndEncodeCertificate(hKey.Get(),
            dwKeySpec,
            X509_ASN_ENCODING,
            X509_CERT_TO_BE_SIGNED,
            &certInfo,
            &certAlgId,
            NULL,
            pbOutput,
            (DWORD*)pcbResult))
        {
            throw TPM_RC_FAILURE;
        }

        // Get the new thumbprint
        DWORD cbNewCertThumbprint = SHA1_DIGEST_SIZE;
        if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM,
            0,
            NULL,
            pbOutput,
            *pcbResult,
            newCertThumbPrint,
            &cbNewCertThumbprint))
        {
            throw TPM_RC_FAILURE;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    // Cleanup
    if (pTpmPublic != NULL)
    {
        LocalFree(pTpmPublic);
        pTpmPublic = NULL;
    }
    if (pbEncAuthorityKeyInfo != NULL)
    {
        LocalFree(pbEncAuthorityKeyInfo);
        pbEncAuthorityKeyInfo = NULL;
    }
    for (UINT32 n = 1; n < certInfo.cExtension; n++) // Not the first one, that is copied
    {
        if (certExtensions[n].Value.pbData != NULL)
        {
            LocalFree(certExtensions[n].Value.pbData);
            certExtensions[n].Value.pbData = NULL;
            certExtensions[n].Value.cbData = 0;
        }
    }
    return result;
}

UINT32 LimpetChallengeIdentityCert(
    __in_bcount(cbEkPub) PBYTE pbEkPub,
    UINT32 cbEkPub,
    __in_bcount(SHA256_DIGEST_SIZE + sizeof(UINT16)) PBYTE idkName,
    __inout_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret,
    __out_bcount_part(cbOutput, *pcbResult) PBYTE pbOutput,
    UINT32 cbOutput,
    __out PUINT32 pcbResult
)
{
    UINT32 result = TPM_RC_SUCCESS;
    BYTE *buffer;
    INT32 size;
    ANY_OBJECT ekPub = { 0 };
    OBJECT wrapObject = { 0 };
    TPM2B_SEED seed = { 0 };
    TPM2B_ENCRYPTED_SECRET secret = { 0 };
    TPM2B_NAME objectName = { 0 };
    TPM2B_DIGEST credential = { 0 };
    TPM2B_ID_OBJECT credentialBlob = { 0 };

    try
    {
        // Re-hydrate the EKPub key object
        buffer = pbEkPub;
        size = cbEkPub;
        if ((result = TPM2B_PUBLIC_Unmarshal(&ekPub.obj.publicArea, &buffer, &size, FALSE)) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        ObjectComputeName(&ekPub.obj.publicArea.t.publicArea, &ekPub.obj.name);

        // Create the wrapped seed
        wrapObject.publicArea = ekPub.obj.publicArea.t.publicArea;
        wrapObject.name = ekPub.obj.name;
        seed.t.size = SHA256_DIGEST_SIZE;
        secret.t.size = sizeof(secret.t.secret);
        if ((result = CryptSecretEncrypt(&wrapObject, "IDENTITY", (TPM2B_DATA*)&seed, &secret)) != TPM_RC_SUCCESS)
        {
            throw result;
        }

        // Make a handy dandy 16 byte random credential that can serve as an AES key if we didn't get one
        BYTE dirty = 0;
        for (UINT32 n = 0; n < MAX_AES_KEY_BYTES; n++) dirty |= activationSecret[n];
        if (!dirty)
        {
            if ((credential.t.size = CryptGenerateRandom(MAX_AES_KEY_BYTES, credential.t.buffer)) != MAX_AES_KEY_BYTES)
            {
                throw TPM_RC_FAILURE;
            }
        }

        // Create the identity key name to be activated
        objectName.t.size = sizeof(UINT16) + SHA256_DIGEST_SIZE;
        MemoryCopy(objectName.t.name, idkName, sizeof(UINT16) + SHA256_DIGEST_SIZE, sizeof(objectName.t.name));

        // Crank out the credential blob
        SecretToCredential(&credential, &objectName, &seed, &ekPub, &credentialBlob);

        // Return the wrapped activation secret if we generated it
        if (!dirty)
        {
            MemoryCopy(activationSecret, credential.t.buffer, credential.t.size, MAX_AES_KEY_BYTES);
        }

        // Return the activation: TPM2B_ID_OBJECT || TPM2B_ENCRYPTED_SECRET
        *pcbResult = sizeof(credentialBlob) + sizeof(secret); // This is a good estimate how much space we will need
        buffer = pbOutput;
        UINT32 cbBuf = 0;
        size = cbOutput;
        cbBuf += TPM2B_DATA_Marshal((TPM2B_DATA*)&credentialBlob, &buffer, &size);
        if (size < 0)
        {
            throw TPM_RC_SIZE;
        }
        cbBuf += TPM2B_DATA_Marshal((TPM2B_DATA*)&secret, &buffer, &size);
        if (size < 0)
        {
            throw TPM_RC_SIZE;
        }
        *pcbResult = cbBuf;
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetActivateIdentityCert(
    __in UINT32 logicalDeviceNumber,
    __in_z LPCWSTR wcAlgorithm,
    __in_bcount(cbActivation) PBYTE pbActivation,
    UINT32 cbActivation,
    __out_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret
)
{
    UINT32 result = TPM_RC_SUCCESS;
    NTSTATUS status = ERROR_SUCCESS;

    try
    {
        // Create the key name for the KSP
        std::wstring keyName(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetReadDeviceId(logicalDeviceNumber, &keyName[0])) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        keyName.resize(keyName.find(L'\0'));
        keyName += std::wstring(wcAlgorithm);
        keyName.push_back(L'\0');

        // Open the key in the PCPKSP
        AutoCloseNcryptProvHandle hAlg;
        if (((status = NCryptOpenStorageProvider(&hAlg, MS_PLATFORM_KEY_STORAGE_PROVIDER, 0)) != ERROR_SUCCESS) ||
            (hAlg.Get() == NULL))
        {
            throw TPM_RC_FAILURE;
        }
        AutoCloseNcryptKeyHandle hKey;
        if (((status = NCryptOpenKey(hAlg.Get(), &hKey, &keyName[0], 0, NCRYPT_MACHINE_KEY_FLAG)) != ERROR_SUCCESS) ||
            (hKey.Get() == NULL))
        {
            throw TPM_RC_FAILURE;
        }

        // Perform the activation in the provider. This will not work with any OS prior to RS2 because of BUG#8826660.
        DWORD cbActivationSecret;
        if (((status = NCryptSetProperty(hKey.Get(), NCRYPT_PCP_TPM12_IDACTIVATION_PROPERTY, pbActivation, cbActivation, 0)) != ERROR_SUCCESS) ||
            ((status = NCryptGetProperty(hKey.Get(), NCRYPT_PCP_TPM12_IDACTIVATION_PROPERTY, activationSecret, MAX_AES_KEY_BYTES, &cbActivationSecret, 0)) != ERROR_SUCCESS) ||
            (cbActivationSecret < MAX_AES_KEY_BYTES))
        {
            throw TPM_RC_FAILURE;
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetCreateHostageKey(
    __in_bcount(cbEkPub) PBYTE pbEkPub,
    UINT32 cbEkPub,
    __in_bcount(cbSrkPub) PBYTE pbSrkPub,
    UINT32 cbSrkPub,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* deviceName,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* url,
    __in_bcount(SHA256_DIGEST_SIZE) PBYTE hmacKey,
    __inout_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret,
    __out_bcount_part(cbOutput, *pcbResult) PBYTE pbOutput,
    UINT32 cbOutput,
    __out PUINT32 pcbResult
)
{
    return LimpetCreateHostageKeyEx(
        pbEkPub,
        cbEkPub,
        pbSrkPub,
        cbSrkPub,
        deviceName,
        url,
        hmacKey,
        SHA256_DIGEST_SIZE,
        activationSecret,
        pbOutput,
        cbOutput,
        pcbResult);
}

UINT32 LimpetCreateHostageKeyEx(
    __in_bcount(cbEkPub) PBYTE pbEkPub,
    UINT32 cbEkPub,
    __in_bcount(cbSrkPub) PBYTE pbSrkPub,
    UINT32 cbSrkPub,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* deviceName,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* url,
    __in_bcount(cbHmacKey) PBYTE hmacKey,
    UINT32 cbHmacKey,
    __inout_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret,
    __out_bcount_part(cbOutput, *pcbResult) PBYTE pbOutput,
    UINT32 cbOutput,
    __out PUINT32 pcbResult
)
{
    UINT32 result = TPM_RC_SUCCESS;
    BYTE *buffer;
    INT32 size;
    ANY_OBJECT ekPub = { 0 };
    ANY_OBJECT SrkPub = { 0 };
    OBJECT wrapObject = { 0 };
    TPM2B_SEED seed = { 0 };
    TPM2B_ENCRYPTED_SECRET secret = { 0 };
    TPM2B_DIGEST credential = { 0 };
    TPM2B_ID_OBJECT credentialBlob = { 0 };

    try
    {
        // Re-hydrate the EKPub key object
        buffer = pbEkPub;
        size = cbEkPub;
        if ((result = TPM2B_PUBLIC_Unmarshal(&ekPub.obj.publicArea, &buffer, &size, FALSE)) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        ObjectComputeName(&ekPub.obj.publicArea.t.publicArea, &ekPub.obj.name);

        // Re-hydrate the SrKPub key object
        buffer = pbSrkPub;
        size = cbSrkPub;
        if ((result = TPM2B_PUBLIC_Unmarshal(&SrkPub.obj.publicArea, &buffer, &size, FALSE)) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        ObjectComputeName(&SrkPub.obj.publicArea.t.publicArea, &SrkPub.obj.name);

        // Make sure that the SRK has all the properties we expect it to have
        TPMT_PUBLIC srk = SrkPub.obj.publicArea.t.publicArea;
        if ((srk.type != TPM_ALG_RSA) ||
            (srk.nameAlg != TPM_ALG_SHA256) ||
            (srk.objectAttributes.fixedTPM != SET) ||
            (srk.objectAttributes.fixedParent != SET) ||
            (srk.objectAttributes.sensitiveDataOrigin != SET) ||
            (srk.objectAttributes.userWithAuth != SET) ||
            (srk.objectAttributes.noDA != SET) ||
            (srk.objectAttributes.restricted != SET) ||
            (srk.objectAttributes.decrypt != SET) ||
            (srk.parameters.rsaDetail.keyBits != MAX_RSA_KEY_BITS) ||
            (srk.parameters.rsaDetail.exponent != 0) ||
            (srk.parameters.rsaDetail.scheme.scheme != TPM_ALG_NULL) ||
            (srk.parameters.rsaDetail.symmetric.algorithm != TPM_ALG_AES) ||
            (srk.parameters.rsaDetail.symmetric.keyBits.aes != 128) ||
            (srk.parameters.rsaDetail.symmetric.mode.aes != TPM_ALG_CFB) ||
            (srk.unique.rsa.t.size != MAX_RSA_KEY_BITS / 8))
        {
            throw result;
        }

        // Create the wrapped seed
        wrapObject.publicArea = ekPub.obj.publicArea.t.publicArea;
        wrapObject.name = ekPub.obj.name;
        seed.t.size = SHA256_DIGEST_SIZE;
        secret.t.size = sizeof(secret.t.secret);
        if ((result = CryptSecretEncrypt(&wrapObject, "IDENTITY", (TPM2B_DATA*)&seed, &secret)) != TPM_RC_SUCCESS)
        {
            throw result;
        }

        // Make a handy dandy 16 byte random credential that can serve as an AES key if we didn't get one
        BYTE dirty = 0;
        for (UINT32 n = 0; n < MAX_AES_KEY_BYTES; n++) dirty |= activationSecret[n];
        if (!dirty)
        {
            if ((credential.t.size = CryptGenerateRandom(MAX_AES_KEY_BYTES, credential.t.buffer)) != MAX_AES_KEY_BYTES)
            {
                throw TPM_RC_FAILURE;
            }
        }

        // Crank out the credential blob for the SRK
        SecretToCredential(&credential, &SrkPub.obj.name, &seed, &ekPub, &credentialBlob);

        // Return the wrapped activation secret if we generated it
        if (!dirty)
        {
            MemoryCopy(activationSecret, credential.t.buffer, credential.t.size, MAX_AES_KEY_BYTES);
        }

        // Create the symmetric Import key blob
        ANY_OBJECT hmacKeyTpm = { 0 };
        hmacKeyTpm.obj.publicArea.t.publicArea.type = TPM_ALG_KEYEDHASH;
        hmacKeyTpm.obj.publicArea.t.publicArea.nameAlg = TPM_ALG_SHA256;
        hmacKeyTpm.obj.publicArea.t.publicArea.objectAttributes.userWithAuth = 1;
        hmacKeyTpm.obj.publicArea.t.publicArea.objectAttributes.noDA = 1;
        hmacKeyTpm.obj.publicArea.t.publicArea.objectAttributes.sign = 1;
        hmacKeyTpm.obj.publicArea.t.publicArea.parameters.keyedHashDetail.scheme.scheme = TPM_ALG_HMAC;
        hmacKeyTpm.obj.publicArea.t.publicArea.parameters.keyedHashDetail.scheme.details.hmac.hashAlg = TPM_ALG_SHA256;

        // Fill out the private portion
        TPMS_SENSITIVE_CREATE sensitiveCreate = { 0 };
        sensitiveCreate.data.t.size = (UINT16)cbHmacKey;
        MemoryCopy(sensitiveCreate.data.t.buffer, hmacKey, cbHmacKey, sizeof(sensitiveCreate.data.t.buffer));
        MemoryRemoveTrailingZeros((TPM2B_AUTH*)&sensitiveCreate.data);

        // Create the symmetric object
        TPMT_SENSITIVE sensitive = { 0 };
        if ((result = CryptCreateObject(&SrkPub,
            &hmacKeyTpm.obj.publicArea.t.publicArea,
            &sensitiveCreate,
            &sensitive)) != TPM_RC_SUCCESS)
        {
            throw result;
        }

        // Create the wrapped key seed
        TPM2B_SEED keySeed = { 0 };
        keySeed.t.size = SHA256_DIGEST_SIZE;
        TPM2B_ENCRYPTED_SECRET inSymSeed = { 0 };
        inSymSeed.t.size = sizeof(inSymSeed.t.secret);
        OBJECT newParent = { 0 };
        newParent.publicArea = SrkPub.obj.publicArea.t.publicArea;
        newParent.name = SrkPub.obj.name;
        if ((result = CryptSecretEncrypt(&newParent, "DUPLICATE", (TPM2B_DATA*)&keySeed, &inSymSeed)) != TPM_RC_SUCCESS)
        {
            throw result;
        }

        // Wrap the private portaion of the key
        ObjectComputeName(&hmacKeyTpm.obj.publicArea.t.publicArea, &hmacKeyTpm.obj.name);
        TPM2B_DATA innerSymKey = *((TPM2B_DATA*)&credential);
        TPMT_SYM_DEF_OBJECT symDef = { TPM_ALG_AES, AES_KEY_SIZES_BITS, TPM_ALG_CFB };
        SensitiveToDuplicate(&sensitive,
            &hmacKeyTpm.obj.name,
            &SrkPub,
            hmacKeyTpm.obj.publicArea.t.publicArea.nameAlg,
            &keySeed,
            &symDef,
            &innerSymKey,
            &hmacKeyTpm.obj.privateArea);

        // Wrap the URI data
        std::wstring uriInfo(url);
        if (deviceName != NULL)
        {
            uriInfo += std::wstring(L"/");
            uriInfo += std::wstring(deviceName);
            uriInfo += std::wstring(L"\0");
        }
        TPM2B_MAX_BUFFER uriContainer = { 0 };
        uriContainer.t.size = (UINT16)((uriInfo.size()) * sizeof(WCHAR)); // Get the URI string including the terminator
        uriInfo.resize((((uriInfo.size()) + 15) / 16) * 16); // Round up to the next full AES block

        // AES-CFB encryption of the data
        TPM2B_IV iv = { 0 };
        iv.t.size = MAX_AES_BLOCK_SIZE_BYTES;
        CryptSymmetricEncrypt(uriContainer.t.buffer, TPM_ALG_AES, credential.t.size * 8, TPM_ALG_CFB, credential.t.buffer, &iv, (ULONG)(uriInfo.size() * sizeof(WCHAR)), (PBYTE)uriInfo.c_str());

        // Return the Hostage Blob: TPM2B_ID_OBJECT || TPM2B_ENCRYPTED_SECRET || TPM2B_PRIVATE || TPM2B_ENCRYPTED_SECRET || TPM2B_PUBLIC || TPM2B_MAX_BUFFER
        *pcbResult = sizeof(credentialBlob) + sizeof(secret) + sizeof(hmacKeyTpm.obj.privateArea) + sizeof(hmacKeyTpm.obj.publicArea) + sizeof(uriContainer); // This is a reasonable estimate how much space we could use at a maximum
        buffer = pbOutput;
        UINT32 cbBuf = 0;
        size = cbOutput;
        cbBuf += TPM2B_ID_OBJECT_Marshal(&credentialBlob, &buffer, &size);
        if (size < 0)
        {
            throw TPM_RC_SIZE;
        }
        cbBuf += TPM2B_ENCRYPTED_SECRET_Marshal(&secret, &buffer, &size);
        if (size < 0)
        {
            throw TPM_RC_SIZE;
        }
        cbBuf += TPM2B_PRIVATE_Marshal(&hmacKeyTpm.obj.privateArea, &buffer, &size);
        if (size < 0)
        {
            throw TPM_RC_SIZE;
        }
        cbBuf += TPM2B_ENCRYPTED_SECRET_Marshal(&inSymSeed, &buffer, &size);
        if (size < 0)
        {
            throw TPM_RC_SIZE;
        }
        cbBuf += TPM2B_PUBLIC_Marshal(&hmacKeyTpm.obj.publicArea, &buffer, &size);
        if (size < 0)
        {
            throw TPM_RC_SIZE;
        }
        cbBuf += TPM2B_MAX_BUFFER_Marshal(&uriContainer, &buffer, &size);
        if (size < 0)
        {
            throw TPM_RC_SIZE;
        }
        *pcbResult = cbBuf;
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

LSTATUS WriteHostageBlobInRegistry(UINT32 logicalDeviceNumber,
    __in_bcount(cbHostageBlob) PBYTE pbHostageBlob,
    UINT32 cbHostageBlob)
{
    const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);
    return SetRegistryWithBinaryData(logicalDeviceNumberPath, REG_KEY_NAME_HOSTAGEBLOB, (BYTE*)pbHostageBlob, (DWORD)cbHostageBlob);
}

#pragma prefast(suppress: __WARNING_EXCESSIVESTACKUSAGE, "WDGVSO:12767047 Code follows TCG/ISO/IEC standards.")
UINT32 LimpetImportHostageKeyEx(
    __in UINT32 logicalDeviceNumber,
    __in_bcount(cbHostageBlob) PBYTE pbHostageBlob,
    __in UINT32 cbHostageBlob,
    __inout_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcKeyName,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcUriName
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        StartAuthSession_In startAuthSession;
        PolicySecret_In policySecret;
        ActivateCredential_In activateCredential;
        Import_In import;
        Load_In load;
        EvictControl_In evictControl;
        FlushContext_In flushContext;
    } tpm_in;
    union
    {
        StartAuthSession_Out startAuthSession;
        PolicySecret_Out policySecret;
        ActivateCredential_Out activateCredential;
        Import_Out import;
        Load_Out load;
        EvictControl_Out evictControl;
        FlushContext_Out flushContext;
    } tpm_out;

    try
    {
        // Take the hostage apart: TPM2B_ID_OBJECT || TPM2B_ENCRYPTED_SECRET || TPM2B_PRIVATE || TPM2B_ENCRYPTED_SECRET || TPM2B_PUBLIC || TPM2B_MAX_BUFFER
        buffer = pbHostageBlob;
        size = cbHostageBlob;
        std::unique_ptr<TPM2B_ID_OBJECT> credentialBlob(new TPM2B_ID_OBJECT);
        std::unique_ptr<TPM2B_ENCRYPTED_SECRET> secret(new TPM2B_ENCRYPTED_SECRET);
        std::unique_ptr<ANY_OBJECT> hostageKey(new ANY_OBJECT);
        std::unique_ptr<TPM2B_ENCRYPTED_SECRET> inSymSeed(new TPM2B_ENCRYPTED_SECRET);
        std::unique_ptr<TPM2B_MAX_BUFFER> uriContainer(new TPM2B_MAX_BUFFER);
        if (((result = TPM2B_ID_OBJECT_Unmarshal(&*credentialBlob, &buffer, &size)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_ENCRYPTED_SECRET_Unmarshal(&*secret, &buffer, &size)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_PRIVATE_Unmarshal(&hostageKey->obj.privateArea, &buffer, &size)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_ENCRYPTED_SECRET_Unmarshal(&*inSymSeed, &buffer, &size)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_PUBLIC_Unmarshal(&hostageKey->obj.publicArea, &buffer, &size, TRUE)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_MAX_BUFFER_Unmarshal(&*uriContainer, &buffer, &size)) != TPM_RC_SUCCESS))
        {
            throw result;
        }
        // Create the EK policy Session
        INITIALIZE_CALL_BUFFERS(TPM2_StartAuthSession, &tpm_in.startAuthSession, &tpm_out.startAuthSession);
        parms.objectTableIn[TPM2_StartAuthSession_HdlIn_TpmKey].obj.handle = TPM_RH_NULL;
        parms.objectTableIn[TPM2_StartAuthSession_HdlIn_Bind].obj.handle = TPM_RH_NULL;
        tpm_in.startAuthSession.nonceCaller.t.size = CryptGenerateRandom(SHA256_DIGEST_SIZE, tpm_in.startAuthSession.nonceCaller.t.buffer);
        tpm_in.startAuthSession.sessionType = TPM_SE_POLICY;
        tpm_in.startAuthSession.symmetric.algorithm = TPM_ALG_NULL;
        tpm_in.startAuthSession.authHash = TPM_ALG_SHA256;
        LIMPET_TPM_CALL(FALSE, TPM2_StartAuthSession);
        std::unique_ptr<SESSION> policySessionEk(new SESSION);
        *policySessionEk = parms.objectTableOut[TPM2_StartAuthSession_HdlOut_SessionHandle].session;

        // Set the session up for Admin role
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_PolicySecret, &tpm_in.policySecret, &tpm_out.policySecret);
        parms.objectTableIn[TPM2_PolicySecret_HdlIn_AuthHandle].entity.handle = TPM_RH_ENDORSEMENT;
        parms.objectTableIn[TPM2_PolicySecret_HdlIn_AuthHandle].entity.authValue = g_EndorsementAuth;
        parms.objectTableIn[TPM2_PolicySecret_HdlIn_PolicySession].session = *policySessionEk;
        LIMPET_TPM_CALL(FALSE, TPM2_PolicySecret);
        *policySessionEk = parms.objectTableIn[TPM2_PolicySecret_HdlIn_PolicySession].session;

        // Perform the activation and close the EK session
        policySessionEk->attributes.continueSession = CLEAR;
        sessionTable[0].handle = TPM_RS_PW;
        sessionTable[1] = *policySessionEk;
        INITIALIZE_CALL_BUFFERS(TPM2_ActivateCredential, &tpm_in.activateCredential, &tpm_out.activateCredential);
        parms.objectTableIn[TPM2_ActivateCredential_HdlIn_ActivateHandle].generic.handle = LIMPET_TPM20_SRK_HANDLE; // SRK is using nullAuth
        parms.objectTableIn[TPM2_ActivateCredential_HdlIn_KeyHandle].generic.handle = LIMPET_TPM20_ERK_HANDLE; // Policy authorized
        tpm_in.activateCredential.secret = *secret;
        tpm_in.activateCredential.credentialBlob = *credentialBlob;
        LIMPET_TPM_CALL(FALSE, TPM2_ActivateCredential);
        std::unique_ptr<TPM2B_DATA> encryptionKey(new TPM2B_DATA);
        MemoryCopy2B((TPM2B*)&*encryptionKey, (TPM2B*)&tpm_out.activateCredential.certInfo, sizeof(encryptionKey->t.buffer));
        // Return the activation secret
        if (encryptionKey->t.size != MAX_AES_KEY_BYTES)
        {
            throw TPM_RC_FAILURE;
        }
        MemoryCopy(activationSecret, encryptionKey->t.buffer, encryptionKey->t.size, MAX_AES_KEY_BYTES);

        // Import the hostage key using the activation secret as inner wrapper under the SRK
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_Import, &tpm_in.import, &tpm_out.import);
        parms.objectTableIn[TPM2_Import_HdlIn_ParentHandle].generic.handle = LIMPET_TPM20_SRK_HANDLE;
        tpm_in.import.encryptionKey = *encryptionKey;
        tpm_in.import.objectPublic = hostageKey->obj.publicArea;
        tpm_in.import.duplicate = hostageKey->obj.privateArea;
        tpm_in.import.inSymSeed = *inSymSeed;
        tpm_in.import.symmetricAlg = { TPM_ALG_AES, AES_KEY_SIZES_BITS, TPM_ALG_CFB };
        LIMPET_TPM_CALL(FALSE, TPM2_Import);
        hostageKey->obj.privateArea = tpm_out.import.outPrivate;

        if (UseRegistry(logicalDeviceNumber))
        {
            const std::wstring logicalDeviceNumberPath = REG_KEY_SUBPATH_DMCLIENT_NTSERVICE + L"\\" + std::to_wstring(logicalDeviceNumber);

            BYTE hmacBuffer[2048];
            INT32 cbhmacBuffer = 2048;
            INT32 actualDataSize = 0;
            std::wstring localwcKeyName(LIMPET_STRING_SIZE, L'\0');

            // write TPM protected hmac public key in registry
            for (INT32 i = 0; i < cbhmacBuffer; i++)
            {
                hmacBuffer[i] = '\0';
            }
            buffer = hmacBuffer;
            size = cbhmacBuffer;
            actualDataSize = TPM2B_PUBLIC_Marshal(&hostageKey->obj.publicArea, &buffer, &size);
            if ((result = SetRegistryWithBinaryData(logicalDeviceNumberPath, REG_KEY_NAME_HMACPUBLICKEYNAME, (BYTE*)hmacBuffer, (DWORD)actualDataSize)) != 0)
            {
                return result;
            }

            // write the TPM protected hmac private key in registry.
            cbhmacBuffer = 2048;
            for (INT32 i = 0; i < cbhmacBuffer; i++)
            {
                hmacBuffer[i] = '\0';
            }
            buffer = hmacBuffer;
            size = cbhmacBuffer;
            actualDataSize = TPM2B_PRIVATE_Marshal(&hostageKey->obj.privateArea, &buffer, &size);
            if ((result = SetRegistryWithBinaryData(logicalDeviceNumberPath, REG_KEY_NAME_HMACPRIVATEKEYNAME, (BYTE*)hmacBuffer, (DWORD)actualDataSize)) != 0)
            {
                return result;
            }

            //write fixed hmac object name in the registry.
            result = CreateStringRegistryKeyValue(logicalDeviceNumberPath, REG_KEY_NAME_HMACOBJECTNAME, FIXED_HMAC_OBJECT_NAME_FOR_REGISTRY);

            // Return the fixed hmac object name.
            if (wcKeyName != nullptr)
            {
                wmemcpy_s(wcKeyName, LIMPET_STRING_SIZE, FIXED_HMAC_OBJECT_NAME_FOR_REGISTRY.c_str(), FIXED_HMAC_OBJECT_NAME_FOR_REGISTRY.size());
            }
        }
        else
        {
            // Load the new domesticated key into the TPM
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_Load, &tpm_in.load, &tpm_out.load);
            parms.objectTableIn[TPM2_Load_HdlIn_ParentHandle].generic.handle = LIMPET_TPM20_SRK_HANDLE;
            tpm_in.load.inPrivate = hostageKey->obj.privateArea;
            tpm_in.load.inPublic = hostageKey->obj.publicArea;
            LIMPET_TPM_CALL(FALSE, TPM2_Load);
            *hostageKey = parms.objectTableOut[TPM2_Load_HdlOut_ObjectHandle];

            // Persist the key in TPM NV storage and it will never ever see the light of day
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_EvictControl, &tpm_in.evictControl, &tpm_out.evictControl);
            parms.objectTableIn[TPM2_EvictControl_HdlIn_Auth].entity.handle = TPM_RH_OWNER;
            parms.objectTableIn[TPM2_EvictControl_HdlIn_Auth].entity.authValue = g_StorageAuth;
            parms.objectTableIn[TPM2_EvictControl_HdlIn_ObjectHandle] = *hostageKey;
            tpm_in.evictControl.persistentHandle = LIMPET_TPM20_PERSISTED_KEY_INDEX + logicalDeviceNumber;
            LIMPET_TPM_CALL(FALSE, TPM2_EvictControl);
            *hostageKey = parms.objectTableIn[TPM2_EvictControl_HdlIn_ObjectHandle];

            // Return the HMAC key objects name
            if (wcKeyName != nullptr)
            {
                result = LimpetObjNameToString(hostageKey->obj.name.t.name, hostageKey->obj.name.t.size, wcKeyName);
            }
        }

        if (result == TPM_RC_SUCCESS)
        {
            // AES-CFB decrypt the payload buffer
            std::vector<BYTE> uriInfo(((uriContainer->t.size + 15) / 16) * 16); // Round the databuffer up to the next full block
            TPM2B_IV iv = { 0 };
            iv.t.size = MAX_AES_BLOCK_SIZE_BYTES;
            CryptSymmetricDecrypt(&uriInfo[0], TPM_ALG_AES, encryptionKey->t.size * 8, TPM_ALG_CFB, encryptionKey->t.buffer, &iv, (ULONG)(((uriContainer->t.size + 15) / 16) * 16), uriContainer->t.buffer);
            uriInfo.resize(uriContainer->t.size + 2);
            *((WCHAR*)(uriInfo.data() + uriContainer->t.size)) = L'\0';

            // Set the URI info
            if ((result = LimpetStoreURI(logicalDeviceNumber, (WCHAR*)&uriInfo[0], wcUriName)) != TPM_RC_SUCCESS)
            {
                throw result;
            }
        }
    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetImportHostageKey(
    __in UINT32 logicalDeviceNumber,
    __in_bcount(cbHostageBlob) PBYTE pbHostageBlob,
    __in UINT32 cbHostageBlob,
    __inout_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcKeyName,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcUriName
)
{
    if (UseRegistry(logicalDeviceNumber) &&
        IsLazyLoadingEnabled())
    {
        return WriteHostageBlobInRegistry(logicalDeviceNumber, pbHostageBlob, cbHostageBlob);
    }
    else
    {
        return  LimpetImportHostageKeyEx(logicalDeviceNumber,
            pbHostageBlob,
            cbHostageBlob,
            activationSecret,
            wcKeyName,
            wcUriName);
    }
}

HRESULT Base64StringToByteArray(
    __in const std::wstring data,
    __out_bcount(*cbbinaryData) PBYTE binaryData,
    __in INT32 *cbbinaryData
)
{
    RETURN_HR_IF_NULL(E_INVALIDARG, binaryData);
    RETURN_HR_IF_NULL(E_INVALIDARG, cbbinaryData);

    // find the size needed for binary array
    DWORD sizeNeeded = 0;
    if (!CryptStringToBinaryW(
        &data[0],
        (DWORD)data.size(),
        CRYPT_STRING_BASE64,
        NULL,
        &sizeNeeded,
        NULL,
        NULL))
    {
        RETURN_HR(E_FAIL);
    }

    RETURN_HR_IF(E_INVALIDARG, *cbbinaryData < (INT32)sizeNeeded);
    std::vector<BYTE> localBinaryData(sizeNeeded);
    if (!CryptStringToBinaryW(
        &data[0],
        (DWORD)data.size(),
        CRYPT_STRING_BASE64,
        &localBinaryData[0],
        &sizeNeeded,
        NULL,
        NULL))
    {
        RETURN_HR(E_FAIL);
    }

    MemoryCopy(binaryData, &localBinaryData[0], sizeNeeded, (DWORD)*cbbinaryData);
    *cbbinaryData = (INT32)sizeNeeded;
    return 0;
}

#pragma prefast(suppress: __WARNING_EXCESSIVESTACKUSAGE, "WDGVSO:12767047 Code follows TCG/ISO/IEC standards.")
UINT32 LimpetSignWithImportedHostageKey(
    __in const std::wstring hostageKey,
    __in_bcount(cbDataToSign) PCBYTE pbDataToSign,
    __in UINT32 cbDataToSign,
    __out_bcount(SHA256_DIGEST_SIZE) PBYTE pbSignature
)
{
    UINT32 result = TPM_RC_SUCCESS;

    std::vector<BYTE> hostageKeyBinaryData(MAX_CONNECTION_STRING_LEN, '\0');
    INT32 cbhostageKeyBinaryData = MAX_CONNECTION_STRING_LEN;
    if (!SUCCEEDED(result = Base64StringToByteArray(hostageKey, &hostageKeyBinaryData[0], &cbhostageKeyBinaryData)))
    {
        return result;
    }
    hostageKeyBinaryData.resize(cbhostageKeyBinaryData);

    std::vector<PBYTE> activationSecret(MAX_AES_KEY_BYTES);

    DEFINE_CALL_BUFFERS;

    union
    {
        StartAuthSession_In startAuthSession;
        PolicySecret_In policySecret;
        ActivateCredential_In activateCredential;
        Import_In import;
        Load_In load;
        HMAC_In hmac;
        FlushContext_In flushContext;
    } tpm_in;
    union
    {
        StartAuthSession_Out startAuthSession;
        PolicySecret_Out policySecret;
        ActivateCredential_Out activateCredential;
        Import_Out import;
        Load_Out load;
        HMAC_Out hmac;
        FlushContext_Out flushContext;
    } tpm_out;

    try
    {
        // Take the hostage apart: TPM2B_ID_OBJECT || TPM2B_ENCRYPTED_SECRET || TPM2B_PRIVATE || TPM2B_ENCRYPTED_SECRET || TPM2B_PUBLIC || TPM2B_MAX_BUFFER
        buffer = &hostageKeyBinaryData[0];
        size = cbhostageKeyBinaryData;
        std::unique_ptr<TPM2B_ID_OBJECT> credentialBlob(new TPM2B_ID_OBJECT);
        std::unique_ptr<TPM2B_ENCRYPTED_SECRET> secret(new TPM2B_ENCRYPTED_SECRET);
        std::unique_ptr<ANY_OBJECT> hostageKey(new ANY_OBJECT);
        std::unique_ptr<TPM2B_ENCRYPTED_SECRET> inSymSeed(new TPM2B_ENCRYPTED_SECRET);
        std::unique_ptr<TPM2B_MAX_BUFFER> uriContainer(new TPM2B_MAX_BUFFER);
        if (((result = TPM2B_ID_OBJECT_Unmarshal(&*credentialBlob, &buffer, &size)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_ENCRYPTED_SECRET_Unmarshal(&*secret, &buffer, &size)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_PRIVATE_Unmarshal(&hostageKey->obj.privateArea, &buffer, &size)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_ENCRYPTED_SECRET_Unmarshal(&*inSymSeed, &buffer, &size)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_PUBLIC_Unmarshal(&hostageKey->obj.publicArea, &buffer, &size, TRUE)) != TPM_RC_SUCCESS) ||
            ((result = TPM2B_MAX_BUFFER_Unmarshal(&*uriContainer, &buffer, &size)) != TPM_RC_SUCCESS))
        {
            throw result;
        }
        // Create the EK policy Session
        INITIALIZE_CALL_BUFFERS(TPM2_StartAuthSession, &tpm_in.startAuthSession, &tpm_out.startAuthSession);
        parms.objectTableIn[TPM2_StartAuthSession_HdlIn_TpmKey].obj.handle = TPM_RH_NULL;
        parms.objectTableIn[TPM2_StartAuthSession_HdlIn_Bind].obj.handle = TPM_RH_NULL;
        tpm_in.startAuthSession.nonceCaller.t.size = CryptGenerateRandom(SHA256_DIGEST_SIZE, tpm_in.startAuthSession.nonceCaller.t.buffer);
        tpm_in.startAuthSession.sessionType = TPM_SE_POLICY;
        tpm_in.startAuthSession.symmetric.algorithm = TPM_ALG_NULL;
        tpm_in.startAuthSession.authHash = TPM_ALG_SHA256;
        LIMPET_TPM_CALL(FALSE, TPM2_StartAuthSession);
        std::unique_ptr<SESSION> policySessionEk(new SESSION);
        *policySessionEk = parms.objectTableOut[TPM2_StartAuthSession_HdlOut_SessionHandle].session;

        // Set the session up for Admin role
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_PolicySecret, &tpm_in.policySecret, &tpm_out.policySecret);
        parms.objectTableIn[TPM2_PolicySecret_HdlIn_AuthHandle].entity.handle = TPM_RH_ENDORSEMENT;
        parms.objectTableIn[TPM2_PolicySecret_HdlIn_AuthHandle].entity.authValue = g_EndorsementAuth;
        parms.objectTableIn[TPM2_PolicySecret_HdlIn_PolicySession].session = *policySessionEk;
        LIMPET_TPM_CALL(FALSE, TPM2_PolicySecret);
        *policySessionEk = parms.objectTableIn[TPM2_PolicySecret_HdlIn_PolicySession].session;

        // Perform the activation and close the EK session
        policySessionEk->attributes.continueSession = CLEAR;
        sessionTable[0].handle = TPM_RS_PW;
        sessionTable[1] = *policySessionEk;
        INITIALIZE_CALL_BUFFERS(TPM2_ActivateCredential, &tpm_in.activateCredential, &tpm_out.activateCredential);
        parms.objectTableIn[TPM2_ActivateCredential_HdlIn_ActivateHandle].generic.handle = LIMPET_TPM20_SRK_HANDLE; // SRK is using nullAuth
        parms.objectTableIn[TPM2_ActivateCredential_HdlIn_KeyHandle].generic.handle = LIMPET_TPM20_ERK_HANDLE; // Policy authorized
        tpm_in.activateCredential.secret = *secret;
        tpm_in.activateCredential.credentialBlob = *credentialBlob;
        LIMPET_TPM_CALL(FALSE, TPM2_ActivateCredential);
        std::unique_ptr<TPM2B_DATA> encryptionKey(new TPM2B_DATA);
        MemoryCopy2B((TPM2B*)&*encryptionKey, (TPM2B*)&tpm_out.activateCredential.certInfo, sizeof(encryptionKey->t.buffer));
        // Return the activation secret
        if (encryptionKey->t.size != MAX_AES_KEY_BYTES)
        {
            throw TPM_RC_FAILURE;
        }
        MemoryCopy(&activationSecret[0], encryptionKey->t.buffer, encryptionKey->t.size, MAX_AES_KEY_BYTES);

        // Import the hostage key using the activation secret as inner wrapper under the SRK
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_Import, &tpm_in.import, &tpm_out.import);
        parms.objectTableIn[TPM2_Import_HdlIn_ParentHandle].generic.handle = LIMPET_TPM20_SRK_HANDLE;
        tpm_in.import.encryptionKey = *encryptionKey;
        tpm_in.import.objectPublic = hostageKey->obj.publicArea;
        tpm_in.import.duplicate = hostageKey->obj.privateArea;
        tpm_in.import.inSymSeed = *inSymSeed;
        tpm_in.import.symmetricAlg = { TPM_ALG_AES, AES_KEY_SIZES_BITS, TPM_ALG_CFB };
        LIMPET_TPM_CALL(FALSE, TPM2_Import);
        hostageKey->obj.privateArea = tpm_out.import.outPrivate;

        // Load the new domesticated key into the TPM
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_Load, &tpm_in.load, &tpm_out.load);
        parms.objectTableIn[TPM2_Load_HdlIn_ParentHandle].generic.handle = LIMPET_TPM20_SRK_HANDLE;
        tpm_in.load.inPrivate = hostageKey->obj.privateArea;
        tpm_in.load.inPublic = hostageKey->obj.publicArea;
        LIMPET_TPM_CALL(FALSE, TPM2_Load);
        *hostageKey = parms.objectTableOut[TPM2_Load_HdlOut_ObjectHandle];

        // HMAC sign the data
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_HMAC, &tpm_in.hmac, &tpm_out.hmac);
        parms.objectTableIn[TPM2_HMAC_HdlIn_Handle] = *hostageKey;
        tpm_in.hmac.hashAlg = TPM_ALG_SHA256;
        MemoryCopy(tpm_in.hmac.buffer.t.buffer, pbDataToSign, cbDataToSign, sizeof(tpm_in.hmac.buffer.t.buffer));
        tpm_in.hmac.buffer.t.size = (UINT16)cbDataToSign;
        LIMPET_TPM_CALL(FALSE, TPM2_HMAC);

        // Return the HMAC
        MemoryCopy(pbSignature, tpm_out.hmac.outHMAC.t.buffer, tpm_out.hmac.outHMAC.t.size, 32);

        // Unload the key from TPM.
        INITIALIZE_CALL_BUFFERS(TPM2_FlushContext, &tpm_in.flushContext, &tpm_out.flushContext);
        parms.objectTableIn[TPM2_FlushContext_HdlIn_FlushHandle] = *hostageKey;
        LIMPET_TPM_CALL(FALSE, TPM2_FlushContext);

    }
    catch (UINT32 err)
    {
        result = err;
    }
    catch (...)
    {
        result = TPM_RC_FAILURE;
    }

    return result;
}

UINT32 LimpetForceClearTpm()
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        GetCapability_In getCapability;
        Clear_In clear;
        ClearControl_In clearControl;
    } tpm_in;
    union
    {
        GetCapability_Out getCapability;
        Clear_Out clear;
        ClearControl_Out clearControl;
    } tpm_out;
    TPM2B_AUTH lockoutAuth = { 0 };
    UINT32 lockoutAuthSize = sizeof(lockoutAuth.t.buffer);

    // Let's poke around before we pull the trigger and see how we can clear most effectively
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_TPM_PROPERTIES;
    tpm_in.getCapability.property = TPM_PT_PERMANENT;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    if ((((TPMA_PERMANENT*)&tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value)->disableClear == 0) && // Is DisableClear not set?
        ((((TPMA_PERMANENT*)&tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value)->lockoutAuthSet == 0) || // Is LockoutAuth not set?
        ((Tbsi_Get_OwnerAuth(g_hTbs, TBS_OWNERAUTH_TYPE_FULL, lockoutAuth.t.buffer, &lockoutAuthSize) == TBS_SUCCESS)))) // If it is set, can we get it from the registry?
    {
        // Tbsi_Get_OwnerAuth would have failed if the auth is longer than sizeof(lockoutAuth.t.buffer)
        lockoutAuth.t.size = (UINT16)lockoutAuthSize;
        sessionTable[0].handle = TPM_RS_PW;
        INITIALIZE_CALL_BUFFERS(TPM2_Clear, &tpm_in.clear, &tpm_out.clear);
        parms.objectTableIn[TPM2_Clear_HdlIn_AuthHandle].entity.handle = TPM_RH_LOCKOUT;
        parms.objectTableIn[TPM2_Clear_HdlIn_AuthHandle].entity.authValue = lockoutAuth;
        // Place the pike carefully at the temple and hit it with the hammer
        LIMPET_TRY_TPM_CALL(FALSE, TPM2_Clear);
    }

    // Well, no such luck... schedule a PPI call with the firmware and hope for the best.
    if (result != TPM_RC_SUCCESS)
    {
        UINT32 pIntsIn[2] = { 2, 22 }; // ScheduleOp 22 (Enable,Activate,Clear,Enable,Activate)
        UINT32 pIntsOut[1] = { 0 };
        UINT32 pIntsOutSize = sizeof(UINT32);
        if (((result = Tbsi_Physical_Presence_Command(g_hTbs, (PCBYTE)pIntsIn, sizeof(pIntsIn), (PBYTE)pIntsOut, &pIntsOutSize)) != TBS_SUCCESS) ||
            (pIntsOut[0] != 0))
        {
            // Last resort: Try a platform clear. This will only work if the platformAuth has not been randomized
            // or the platform hierarchy was disabled, so typically for add-on TPMs
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_ClearControl, &tpm_in.clearControl, &tpm_out.clearControl);
            parms.objectTableIn[TPM2_ClearControl_HdlIn_Auth].entity.handle = TPM_RH_PLATFORM;
            tpm_in.clearControl.disable = NO;
            LIMPET_TPM_CALL(FALSE, TPM2_ClearControl);
            sessionTable[0].handle = TPM_RS_PW;
            INITIALIZE_CALL_BUFFERS(TPM2_Clear, &tpm_in.clear, &tpm_out.clear);
            parms.objectTableIn[TPM2_Clear_HdlIn_AuthHandle].entity.handle = TPM_RH_PLATFORM;
            LIMPET_TPM_CALL(FALSE, TPM2_Clear);
        }
    }

    return result;
}

static std::wstring Azure_EncodeBase32(std::vector<unsigned char> &pData)
{
    size_t cbData = pData.size();
    const wchar_t BASE32_VALUES[] = { L"abcdefghijklmnopqrstuvwxyz234567=" };
    const int encodingBlockSize = 5;
    size_t encodedDataLength = (((cbData + encodingBlockSize - 1) / encodingBlockSize) * 8);
    std::vector<wchar_t> bEncodedData(encodedDataLength, 0);

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
    return std::wstring(bEncodedData.begin(), bEncodedData.end());
}

UINT32 LimpetGetRegistrationId(
    std::wstring& registrationId
    )
{
    TPM_RC result = TPM_RC_SUCCESS;
    BCRYPT_ALG_HANDLE       algHandle = INVALID_HANDLE_VALUE;
    BCRYPT_HASH_HANDLE      hHash = INVALID_HANDLE_VALUE;
    DWORD                   digestSize = 0, cbData = 0;
    std::vector<unsigned char>   digest(64, 0);

    std::vector<BYTE> ek(1024);
    UINT32 cbKeyBuffer = (UINT32)ek.size();

    // Get the public key blob
    if ((result = LimpetGetTpmKey2BPub(LIMPET_TPM20_ERK_HANDLE, &ek[0], (UINT32)ek.size(), &cbKeyBuffer, NULL)) != TPM_RC_SUCCESS)
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
    else if ((result = BCryptHashData(hHash, &ek[0], from_sizet<ULONG>(cbKeyBuffer), 0)) != 0)
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
