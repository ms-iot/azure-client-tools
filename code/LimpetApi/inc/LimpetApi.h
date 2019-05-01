// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "LimpetDefs.h"

#define CPG_UTF8 65001

// Number of seconds from 1 Jan. 1601 00:00 to 1 Jan 1970 00 : 00 UTC
#define WINDOWS_TICKS_PER_SEC 10000000
#define EPOCH_DIFFERENCE 11644473600LL

// Because this macro has not yet made it into URCHIN
#define LIMPET_TRY_TPM_CALL(__CloseContext, __CommandType) \
if((cbCmd = ##__CommandType##_Marshal(sessionTable, sessionCnt, &parms, &buffer, &size)) == 0) \
{ \
    result = TPM_RC_FAILURE; \
} \
else \
{ \
    if((result = PlatformSubmitTPM20Command(__CloseContext, pbCmd, cbCmd, pbRsp, sizeof(pbRsp), &cbRsp)) == TPM_RC_SUCCESS) \
    { \
        buffer = pbRsp; \
        size = cbRsp; \
        result = ##__CommandType##_Unmarshal(sessionTable, sessionCnt, &parms, &buffer, &size); \
    } \
} \

#define LIMPET_TPM_CALL(__CloseContext, __CommandType) \
if((cbCmd = ##__CommandType##_Marshal(sessionTable, sessionCnt, &parms, &buffer, &size)) == 0) \
{ \
    result = TPM_RC_FAILURE; \
} \
else \
{ \
    if((result = PlatformSubmitTPM20Command(__CloseContext, pbCmd, cbCmd, pbRsp, sizeof(pbRsp), &cbRsp)) == TPM_RC_SUCCESS) \
    { \
        buffer = pbRsp; \
        size = cbRsp; \
        result = ##__CommandType##_Unmarshal(sessionTable, sessionCnt, &parms, &buffer, &size); \
    } \
} \
if (result != TPM_RC_SUCCESS) \
{ \
    throw result; \
} \

typedef const unsigned char* PCBYTE;
extern UINT32 g_Limpet_LastTpmCmd;
extern UINT32 g_Limpet_LastTpmError;

UINT32 LimpetSanitizeString(
    __in_ecount_z(LIMPET_STRING_SIZE) const WCHAR* szHmac,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* szHmacClean
);

UINT32 LimpetObjNameToString(
    __in_bcount(nameLen) BYTE* name,
    __in UINT16 nameLen,
    __out_z WCHAR* nameStr
);

UINT32 LimpetInitialize();

UINT32 LimpetDestroy();

UINT32 LimpetGetTpmKey2BPubFromCert(
    __in_bcount(cbCert) PBYTE pbCert,
    UINT32 cbCert,
    __out_bcount_part(cbIdKeyPub, *pcbIdKeyPub) PBYTE pbIdKeyPub,
    __in UINT32 cbIdKeyPub,
    __out PUINT32 pcbIdKeyPub,
    __out_bcount_opt(SHA256_DIGEST_SIZE + sizeof(UINT16)) PBYTE idKeyName
);

UINT32 LimpetGetTpmKey2BPub(
    __in UINT32 tpmKeyHandle,
    __out_bcount_part(cbPubKey, *pcbPubKey) PBYTE pbPubKey,
    __in UINT32 cbPubKey,
    __out PUINT32 pcbPubKey,
    __out_bcount_opt(SHA256_DIGEST_SIZE + sizeof(UINT16)) PBYTE pubKeyName
);

UINT32 LimpetReadDeviceId(
    __in UINT32 LogicalDeviceNumber,
    __out_ecount_z(LIMPET_DEVICE_ID_STRING_LENGTH) WCHAR* wcDeviceId
);

UINT32 LimpetStoreURI(
    __in UINT32 logicalDeviceNumber,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* szURI,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
);

UINT32 LimpetReadURI(
    __in UINT32 logicalDeviceNumber,
    __out_bcount_part(cbUriData, *pcbUriData) PBYTE pbUriData,
    __in UINT32 cbUriData,
    __out PUINT32 pcbUriData
);

UINT32 LimpetDestroyURI(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
);

UINT32 LimpetCreateHmacKey(
    __in UINT32 logicalDeviceNumber,
    __in_bcount(cbKey) PBYTE pbKey,
    __in UINT32 cbKey,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
);

UINT32 LimpetEvictHmacKey(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
);

UINT32 LimpetIsLogicalDevicePresent(
    __in UINT32 logicalDeviceNumber,
    __out PBOOL exist
);

UINT32 LimpetSymmetricPayloadProtection(
    __in bool encrypt,
    __in_bcount(MAX_AES_KEY_BYTES) PBYTE aes128Key,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* wcfileName
);

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
);

UINT32 LimpetDestroyIdentityCert(
    __in UINT32 logicalDeviceNumber,
    __in_z LPCWSTR wcAlgorithm,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
);

UINT32 LimpetImportIdentityCert(
    __in_bcount(cbNewCert) PBYTE pbNewCert,
    __in UINT32 cbNewCert,
    __out_bcount(SHA1_DIGEST_SIZE) PBYTE certThumbPrint,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcName
);

UINT32 LimpetSignWithIdentityCert(
    __in_bcount(SHA1_DIGEST_SIZE) PBYTE certThumbPrint,
    __in_bcount(cbDataToSign) PBYTE pbDataToSign,
    __in UINT32 cbDataToSign,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* subjectName,
    __out_bcount_part(cbSignature, *pcbSignature) PBYTE pbSignature,
    __in UINT32 cbSignature,
    __out PUINT32 pcbSignature
);

UINT32 LimpetSignWithIdentityHmacKey(
    __in UINT32 LogicalDeviceNumber,
    __in_bcount(cbDataToSign) PCBYTE pbDataToSign,
    __in UINT32 cbDataToSign,
    __out_bcount(SHA256_DIGEST_SIZE) PBYTE pbSignature
);

UINT32 LimpetSignWithImportedHostageKey(
    __in const std::wstring hostageKey,
    __in_bcount(cbDataToSign) PCBYTE pbDataToSign,
    __in UINT32 cbDataToSign,
    __out_bcount(SHA256_DIGEST_SIZE) PBYTE pbSignature
);

UINT32 LimpetGenerateSASToken(
    __in UINT32 logicalDeviceNumber,
    __in LARGE_INTEGER* expiration,
    __out_ecount_z(*pcchConnectionString) WCHAR* szConnectionString,
    _Inout_ size_t *pcchConnectionString
);

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
);

UINT32 LimpetChallengeIdentityCert(
    __in_bcount(cbEkPub) PBYTE pbEkPub,
    UINT32 cbEkPub,
    __in_bcount(SHA256_DIGEST_SIZE + sizeof(UINT16)) PBYTE idkName,
    __inout_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret,
    __out_bcount_part(cbOutput, *pcbResult) PBYTE pbOutput,
    UINT32 cbOutput,
    __out PUINT32 pcbResult
);

UINT32 LimpetActivateIdentityCert(
    __in UINT32 logicalDeviceNumber,
    __in_z LPCWSTR wcAlgorithm,
    __in_bcount(cbActivation) PBYTE pbActivation,
    UINT32 cbActivation,
    __out_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret
);

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
);

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
);

UINT32 LimpetImportHostageKey(
    __in UINT32 logicalDeviceNumber,
    __in_bcount(cbHostageBlob) PBYTE pbHostageBlob,
    UINT32 cbHostageBlob,
    __inout_bcount(MAX_AES_KEY_BYTES) PBYTE activationSecret,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcKeyName,
    __out_ecount_z_opt(LIMPET_STRING_SIZE) WCHAR* wcUriName
);

UINT32 LimpetReadHmacName(
    UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* name
);

UINT32 LimpetReadUriName(UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* name
);

UINT32 LimpetForceClearTpm();

UINT32 LimpetGetRegistrationId(
     std::wstring& registrationId
);
