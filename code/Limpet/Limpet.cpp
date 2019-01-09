// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ACPITABLs.h"
#include "..\AzureDeviceProvisioningClient\inc\DpsClientApi.h"

#define PARAMETER_COMMAND_ENROLLMENTINFO        L"enrollmentinfo"
#define PARAMETER_COMMAND_REGISTER              L"register"

#define PARAMETER_SWITCH_FORMAT_CSV             L"csv"
#define PARAMETER_SWITCH_FORMAT_JSON            L"json"
#define PARAMETER_SWITCH_FORMAT_TXT             L"txt"

#define PARAMETER_SWITCH_AZUREDPS               L"azuredps"
#define PARAMETER_SWITCH_HELP                   L"help"

// Global variables around the TPM on the system
BOOL TpmFound = TRUE;
TPM_DEVICE_INFO deviceInfo = { 0 };
TBS_CONTEXT_PARAMS2 context = { TBS_CONTEXT_VERSION_TWO,{ 0, 0, 1 } };
std::vector<std::wstring> g_parametersLC;

using namespace Microsoft::Azure::DeviceManagement::Utils;

class LimpetPlatformInitialize
{
public:
    LimpetPlatformInitialize()
    {
        LimpetInitialize();
        // Prepare the output and prepare restorarion when we exit
        oldCodePage = GetConsoleCP();
        SetConsoleOutputCP(CPG_UTF8);
    }

    ~LimpetPlatformInitialize()
    {
        SetConsoleOutputCP(oldCodePage);
        LimpetDestroy();
    }
private:
    UINT32 oldCodePage = 0;
};

static WCHAR charToLower(
    WCHAR c
)
{
    return (WCHAR)::tolower(c);
}

static std::wstring WStringToLower(
    std::wstring& stringIn
)
{
    std::transform(stringIn.begin(), stringIn.end(), stringIn.begin(), charToLower);
    return stringIn;
}

static std::vector<BYTE> ReadBinary(
    const std::wstring& stringIn,
    DWORD flag
)
{
    DWORD cbBinary = 0;
    if (!CryptStringToBinaryW(stringIn.c_str(),
        (DWORD)stringIn.length(),
        flag,
        NULL,
        &cbBinary,
        nullptr,
        nullptr))
    {
        throw TPM_RC_FAILURE;
    }
    std::vector<BYTE> binaryData(cbBinary);
    if (!CryptStringToBinaryW(stringIn.c_str(),
        (DWORD)stringIn.length(),
        flag,
        &binaryData[0],
        &cbBinary,
        nullptr,
        nullptr))
    {
        throw TPM_RC_FAILURE;
    }

    return binaryData;
}

static PCCERT_CONTEXT ReadEncodedCert(
    const std::wstring& fileName
)
{
    DWORD fileSize = 0;
    DWORD read = 0;

    // Read the cert into a string
    AutoCloseHandle hFile = ::CreateFileW(fileName.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile.Get() == NULL)
    {
        throw TPM_RC_FAILURE;
    }
    if ((fileSize = GetFileSize(hFile.Get(), NULL)) == 0)
    {
        throw TPM_RC_FAILURE;
    }
    std::string certString(fileSize + 1, L'\0');
    if (!ReadFile(hFile.Get(), (LPVOID)certString.c_str(), (DWORD)certString.capacity(), &read, nullptr))
    {
        throw TPM_RC_FAILURE;
    }

    // See if the cert is Base64 encoded
    std::vector<BYTE> binaryCert;
    DWORD cbBinary = 0;
    if (CryptStringToBinaryA(certString.c_str(),
        (DWORD)certString.length(),
        CRYPT_STRING_BASE64HEADER,
        NULL,
        &cbBinary,
        nullptr,
        nullptr))
    {
        binaryCert.resize(cbBinary);
        if (!CryptStringToBinaryA(certString.c_str(),
            (DWORD)certString.length(),
            CRYPT_STRING_BASE64HEADER,
            &binaryCert[0],
            &cbBinary,
            nullptr,
            nullptr))
        {
            throw TPM_RC_FAILURE;
        }
    }
    else
    {
        // Maybe it is stored in the DER encoded form
        binaryCert.resize(read);
        MemoryCopy(&binaryCert[0], certString.c_str(), read, (UINT32)binaryCert.size());
    }

    PCCERT_CONTEXT cert = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, &binaryCert[0], (DWORD)binaryCert.size());
    if (cert == NULL)
    {
        throw TPM_RC_FAILURE;
    }

    return cert;
}

static std::wstring MakePrintable(
    const std::vector<BYTE>& dataIn,
    DWORD flag
)
{
    DWORD cchPrintableData = 0;
    std::wstring printableData;

    if (CryptBinaryToStringW(&dataIn[0], (DWORD)dataIn.size(), flag | CRYPT_STRING_NOCRLF, NULL, &cchPrintableData) == FALSE)
    {
        throw TPM_RC_FAILURE;
    }
    printableData.resize(cchPrintableData);
    if (CryptBinaryToStringW(&dataIn[0], (DWORD)dataIn.size(), flag | CRYPT_STRING_NOCRLF, (LPWSTR)printableData.c_str(), &cchPrintableData) == FALSE)
    {
        throw TPM_RC_FAILURE;
    }
    return printableData;
}

static void DumpCertInfo(
    std::wstring prefix,
    PCCERT_CONTEXT cert
)
{
    // Decode the subject name from the cert
    std::wstring subjectName(LIMPET_STRING_SIZE, L'\0');
    if (!CertNameToStrW(X509_ASN_ENCODING, &cert->pCertInfo->Subject, CERT_X500_NAME_STR, (LPWSTR)subjectName.c_str(), (DWORD)subjectName.capacity()))
    {
        throw TPM_RC_FAILURE;
    }
    subjectName.resize(subjectName.find(L'\0'));

    // Decode the issuer name from the cert
    std::wstring issuerName(LIMPET_STRING_SIZE, L'\0');
    if (!CertNameToStr(X509_ASN_ENCODING, &cert->pCertInfo->Issuer, CERT_X500_NAME_STR, (LPWSTR)issuerName.c_str(), (DWORD)issuerName.capacity()))
    {
        throw TPM_RC_FAILURE;
    }
    issuerName.resize(issuerName.find(L'\0'));

    // Encode the thumbprint of the cert
    std::vector<BYTE> thumbprint(SHA1_DIGEST_SIZE, 0);
    DWORD cbThumbprint = (DWORD)thumbprint.size();
    if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM,
        0,
        NULL,
        cert->pbCertEncoded,
        cert->cbCertEncoded,
        &thumbprint[0],
        &cbThumbprint))
    {
        throw TPM_RC_FAILURE;
    }
    std::wstring printableThumbprint = MakePrintable(thumbprint, CRYPT_STRING_HEXRAW);

    // Get key container name in KSP if the cert has an associated key
    std::wstring containerName;
    std::wstring storageProvider;
    DWORD cbKeyProvInfo = sizeof(CRYPT_KEY_PROV_INFO);
    if ((CertGetCertificateContextProperty(cert, CERT_KEY_PROV_INFO_PROP_ID, NULL, &cbKeyProvInfo)) &&
        (cbKeyProvInfo > 0))
    {
        std::vector<BYTE> keyProvInfo(cbKeyProvInfo);
        if (CertGetCertificateContextProperty(cert, CERT_KEY_PROV_INFO_PROP_ID, &keyProvInfo[0], &cbKeyProvInfo))
        {
            PCRYPT_KEY_PROV_INFO pKeyProvInfo = (PCRYPT_KEY_PROV_INFO)&keyProvInfo[0];
            containerName = std::wstring(pKeyProvInfo->pwszContainerName);
            storageProvider = std::wstring(pKeyProvInfo->pwszProvName);
        }
    }

    // Show the entry
    if (!subjectName.empty())
    {
        wprintf(L"%s<Subject>\n%s  %s\n%s</Subject>\n", prefix.c_str(), prefix.c_str(), subjectName.c_str(), prefix.c_str());
    }
    else
    {
        wprintf(L"%s<Subject/>\n", prefix.c_str());
    }
    if (!issuerName.empty())
    {
        wprintf(L"%s<Issuer>\n%s  %s\n%s</Issuer>\n", prefix.c_str(), prefix.c_str(), issuerName.c_str(), prefix.c_str());
    }
    else
    {
        wprintf(L"%s<Issuer/>\n", prefix.c_str());
    }
    wprintf(L"%s<Thumbprint>\n%s  %s\n%s</Thumbprint>\n", prefix.c_str(), prefix.c_str(), printableThumbprint.c_str(), prefix.c_str());
    if (!storageProvider.empty())
    {
        wprintf(L"%s<StorageProvider>\n%s  %s\n%s</StorageProvider>\n", prefix.c_str(), prefix.c_str(), storageProvider.c_str(), prefix.c_str());
    }
    if (!containerName.empty())
    {
        wprintf(L"%s<ContainerName>\n%s  %s\n%s</ContainerName>\n", prefix.c_str(), prefix.c_str(), containerName.c_str(), prefix.c_str());
    }
}

static std::vector<BYTE> ReadFromDisk(
    const std::wstring& fileName,
    DWORD creationDisposition = OPEN_EXISTING
)
{
    DWORD fileSize = 0;
    DWORD read = 0;
    std::vector<BYTE> fileData;

    AutoCloseHandle hFile = ::CreateFileW(fileName.c_str(), GENERIC_READ, 0, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile.Get() == NULL)
    {
        throw TPM_RC_FAILURE;
    }
    if ((fileSize = GetFileSize(hFile.Get(), NULL)) == 0)
    {
        throw TPM_RC_FAILURE;
    }
    fileData.resize(fileSize);
    if (!ReadFile(hFile.Get(), &fileData[0], (DWORD)fileData.size(), &read, nullptr))
    {
        throw TPM_RC_FAILURE;
    }

    return fileData;
}

static void WriteToDisk(
    const std::wstring& fileName,
    const std::vector<BYTE>& fileData,
    DWORD creationDisposition = CREATE_ALWAYS
)
{
    DWORD written = 0;

    AutoCloseHandle hFile = ::CreateFileW(fileName.c_str(), GENERIC_WRITE, 0, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (!WriteFile(hFile.Get(), &fileData[0], (DWORD)fileData.size(), &written, nullptr))
    {
        throw TPM_RC_FAILURE;
    }
}

// Read URI string from the TPM
static void ReadURI(
    __in UINT32 logicalDeviceNumber,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* szURI
)
{
    UINT32 result = TPM_RC_SUCCESS;

    // Read the data from the TPM if we have any
    std::vector<BYTE> nvData(1024);
    UINT32 cbNvData;
    if ((result = LimpetReadURI(logicalDeviceNumber, &nvData[0], (UINT32)nvData.size(), &cbNvData)) == TPM_RC_SUCCESS)
    {
        nvData.resize(cbNvData);

        // Convert back to wide char so we can print it
        if (MultiByteToWideChar(CP_UTF8, 0, (LPCCH)&nvData[0], (UINT32)nvData.size(), szURI, LIMPET_STRING_SIZE) == 0)
        {
            throw TPM_RC_FAILURE;
        }
    }
}

// Enumerate all defined logical devices and their assets on the TPM
static void ListLogicalDevices(
    void
)
{
    NTSTATUS status = ERROR_SUCCESS;
    UINT32 result = TPM_RC_SUCCESS;

    // Open PCPKSP
    AutoCloseNcryptProvHandle hAlg;
    if (((status = NCryptOpenStorageProvider(&hAlg, MS_PLATFORM_KEY_STORAGE_PROVIDER, 0)) != ERROR_SUCCESS) ||
        (hAlg.Get() == NULL))
    {
        throw TPM_RC_FAILURE;
    }

    std::wstring uriName(LIMPET_STRING_SIZE, L'\0');
    std::wstring hmacKeyName(LIMPET_STRING_SIZE, L'\0');
    UINT32 hmacReadResult, uriReadResult;

    // Iterate over all logical device slots
    for (UINT32 n = 0; n < LIMPET_TPM20_MAX_LOGICAL_DEVICE; n++)
    {
        // See if we have asymmetric keys
        std::wstring keyName(LIMPET_STRING_SIZE, L'\0'); \
            if ((result = LimpetReadDeviceId(n, (WCHAR*)keyName.c_str())) != TPM_RC_SUCCESS)
            {
                throw result;
            }
        keyName.resize(keyName.find(L'\0'));

        // See if we have a RSASSA key
        std::wstring rsaKeyName(keyName);
        rsaKeyName += std::wstring(BCRYPT_RSA_ALGORITHM);
        rsaKeyName.push_back(L'\0');
        AutoCloseNcryptKeyHandle hKey;
        if (((status = NCryptOpenKey(hAlg.Get(), &hKey, rsaKeyName.c_str(), 0, NCRYPT_MACHINE_KEY_FLAG)) == ERROR_SUCCESS) ||
            (hKey.Get() != NULL))
        {
            hKey.Close();
        }
        else
        {
            rsaKeyName.clear();
        }

        // See if we have an ECDSA key
        std::wstring eccKeyName(keyName);
        eccKeyName += std::wstring(BCRYPT_ECDSA_P256_ALGORITHM);
        eccKeyName.push_back(L'\0');
        if (((status = NCryptOpenKey(hAlg.Get(), &hKey, eccKeyName.c_str(), 0, NCRYPT_MACHINE_KEY_FLAG)) == ERROR_SUCCESS) ||
            (hKey.Get() != NULL))
        {
            hKey.Close();
        }
        else
        {
            eccKeyName.clear();
        }

        uriReadResult = LimpetReadUriName(n, &uriName[0]);
        hmacReadResult = LimpetReadHmacName(n, &hmacKeyName[0]);

        // If we have a URI or key show it
        if ((uriReadResult == TPM_RC_SUCCESS) ||
            (hmacReadResult == TPM_RC_SUCCESS) ||
            (rsaKeyName.length() > 0) ||
            (eccKeyName.length() > 0))
        {
            wprintf(L"  <LogicalDevice id=\"%d\">\n", n);

            if (uriReadResult == TPM_RC_SUCCESS)
            {
                wprintf(L"    <StorageName>\n      %s\n    </StorageName>\n", uriName.c_str());
            }
            if (hmacReadResult == TPM_RC_SUCCESS)
            {
                wprintf(L"    <HmacKeyName>\n      %s\n    </HmacKeyName>\n", hmacKeyName.c_str());
            }
            if (rsaKeyName.length() != 0)
            {
                wprintf(L"    <RsassaKeyName>\n      %s\n    </RsassaKeyName>\n", rsaKeyName.c_str());
            }
            if (eccKeyName.length() != 0)
            {
                wprintf(L"    <EcdsaKeyName>\n      %s\n    </EcdsaKeyName>\n", eccKeyName.c_str());
            }
            wprintf(L"  </LogicalDevice>\n");
        }
    }
}

// Create (better import) a HMAC key in the TPM and persist it
static void CreateHmacKey(
    __in UINT32 logicalDeviceNumber,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* szKey,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* szName
)
{
    UINT32 result = TPM_RC_SUCCESS;

    // Decode the key
    std::vector<BYTE> hmacKey(SHA256_DIGEST_SIZE * 2);
    DWORD cbHmacKey = (DWORD)hmacKey.size();
    if ((!CryptStringToBinaryW(szKey, (DWORD)wcslen(szKey), CRYPT_STRING_BASE64, &hmacKey[0], &cbHmacKey, nullptr, nullptr)) ||
        (cbHmacKey < SHA256_DIGEST_SIZE / 2) ||
        (cbHmacKey > SHA256_DIGEST_SIZE * 2))
    {
        throw TPM_RC_FAILURE;
    }
    hmacKey.resize(cbHmacKey);

    // Create the key
    if ((result = LimpetCreateHmacKey(logicalDeviceNumber, &hmacKey[0], (UINT32)hmacKey.size(), szName)) != TPM_RC_SUCCESS)
    {
        throw result;
    }
}

// Make a symmetric signature with the HMAC key
static void SignWithHmacKey(
    __in UINT32 logicalDeviceNumber,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* szDataFile,
    __out_ecount_z(LIMPET_STRING_SIZE) WCHAR* szHmac
)
{
    UINT32 result = TPM_RC_SUCCESS;
    DWORD cchHmac = LIMPET_STRING_SIZE;

    // Open the file and read the data
    std::wstring dataFileName(szDataFile);
    std::vector<BYTE> dataToSign = ReadFromDisk(dataFileName);

    // Feed the data through the grinder and return the encoded HMAC
    std::vector<BYTE> hmac(SHA256_DIGEST_SIZE);
    if (((result = LimpetSignWithIdentityHmacKey(logicalDeviceNumber, &dataToSign[0], (UINT32)dataToSign.size(), &hmac[0])) != TPM_RC_SUCCESS) ||
        (!CryptBinaryToStringW(&hmac[0], (DWORD)hmac.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, szHmac, &cchHmac)))
    {
        throw TPM_RC_FAILURE;
    }
}

// Get some information about the TPM and the platform
static void GetTpmInfo(
    void
)
{
    DEFINE_CALL_BUFFERS;
    UINT32 result = TPM_RC_SUCCESS;
    union
    {
        GetCapability_In getCapability;
        TestParms_In testParms;
    } tpm_in;
    union
    {
        GetCapability_Out getCapability;
        TestParms_Out testParms;
    } tpm_out;
    UINT32 value = 0;

    // Start our compound tag
    wprintf(L"<TpmInfo>\n");

    // TPM type
    switch (deviceInfo.tpmInterfaceType)
    {
    case TPM_IFTYPE_1:
        wprintf(L"  <TpmType>DiscreteMMIO</TpmType>\n");
        break;
    case TPM_IFTYPE_TRUSTZONE:
        wprintf(L"  <TpmType>FirmwareTZ</TpmType>\n");
        break;
    case TPM_IFTYPE_HW:
        wprintf(L"  <TpmType>FirmwareTEE</TpmType>\n");
        break;
    case TPM_IFTYPE_EMULATOR:
        wprintf(L"  <TpmType>Emulator(NoSecurity)</TpmType>\n");
        break;
    case TPM_IFTYPE_SPB:
        wprintf(L"  <TpmType>DiscreteSPB</TpmType>\n");
        break;
    default:
        wprintf(L"  <TpmType>Unknown</TpmType>\n");
        break;
    }

    // TPM Family
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_TPM_PROPERTIES;
    tpm_in.getCapability.property = TPM_PT_FAMILY_INDICATOR;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    value = tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value;
    wprintf(L"  <TpmFamily>");
    for (BYTE n = 0; n < sizeof(DWORD); n++)
    {
        char outputChar = ((char*)(&value))[sizeof(DWORD) - 1 - n];
        if ((outputChar > ' ') && (outputChar <= '~'))
        {
            wprintf(L"%c", outputChar);
        }
    }
    wprintf(L"</TpmFamily>\n");

    // TPM Level
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_TPM_PROPERTIES;
    tpm_in.getCapability.property = TPM_PT_LEVEL;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    value = tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value;
    wprintf(L"  <TpmLevel>%d</TpmLevel>\n", value);

    // TPM Revision
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_TPM_PROPERTIES;
    tpm_in.getCapability.property = TPM_PT_REVISION;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    value = tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value;
    wprintf(L"  <TpmRevision>%d.%d</TpmRevision>\n", (value / 100), (value % 100));

    // TPM Manufacturer string
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_TPM_PROPERTIES;
    tpm_in.getCapability.property = TPM_PT_MANUFACTURER;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    value = tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value;
    wprintf(L"  <TpmManufacturer>");
    for (BYTE n = 0; n < sizeof(DWORD); n++)
    {
        char outputChar = ((char*)(&value))[sizeof(DWORD) - 1 - n];
        if ((outputChar > ' ') && (outputChar <= '~'))
        {
            wprintf(L"%c", outputChar);
        }
    }
    wprintf(L"</TpmManufacturer>\n");

    // 16 character TPM Vendor string
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_TPM_PROPERTIES;
    tpm_in.getCapability.property = TPM_PT_VENDOR_STRING_1;
    tpm_in.getCapability.propertyCount = 4;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    wprintf(L"  <TpmVendorString>");
    for (BYTE m = 0; m < 4; m++)
    {
        for (BYTE n = 0; n < sizeof(UINT32); n++)
        {
            char outputChar = (char)((tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[m].value >> ((3 - n) * 8)) & 0x000000ff);
            if (outputChar != '\0')
            {
                wprintf(L"%c", outputChar);
            }
            else
            {
                // String was terminated, let's get out of here
                n = sizeof(UINT32);
                m = 4;
            }
        }
    }
    wprintf(L"</TpmVendorString>\n");

    // TPM Manufacturer Firmware version
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_TPM_PROPERTIES;
    tpm_in.getCapability.property = TPM_PT_FIRMWARE_VERSION_1;
    tpm_in.getCapability.propertyCount = 2;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);

    if (value == 0x49465800) // Manufacturer == IFX
    {
        /*
        https://www.bsi.bund.de/SharedDocs/Downloads/DE/BSI/Zertifizierung/Reporte/Reporte1000/1020a_pdf.pdf?__blob=publicationFile&v=2 Page 14/44
        Byte 1: reserved for future use(0x00)
        Bytes 2 and 3 : Build number(for instance, 0x0A01)
        Byte 4 : Common Criteria certification state, 0x00 means
        TPM is CC certified, 0x02 means TPM is not certified
        */
        wprintf(L"  <TpmFirmware>%d.%d.%d%s</TpmFirmware>\n",
            (tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value >> 16),
            (tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value & 0x0000ffff),
            ((tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[1].value >> 8) & 0x0000ffff),
            (((tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[1].value & 0x000000ff) == 0x00) ? L"-CC" : L""));
    }
    else // All others
    {
        wprintf(L"  <TpmFirmware>%d.%d.%d.%d</TpmFirmware>\n",
            (tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value >> 16),
            (tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value & 0x0000ffff),
            (tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[1].value >> 16),
            (tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[1].value & 0x0000ffff));
    }

    // AuthValues and other interesting stuff
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_TPM_PROPERTIES;
    tpm_in.getCapability.property = TPM_PT_PERMANENT;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    value = tpm_out.getCapability.capabilityData.data.tpmProperties.tpmProperty[0].value;
    if (((TPMA_PERMANENT*)&value)->lockoutAuthSet != 0)
    {
        UINT32 authSize = 0;
        BOOL available = ((Tbsi_Get_OwnerAuth(g_hTbs, TBS_OWNERAUTH_TYPE_FULL, nullptr, &authSize) == TBS_SUCCESS) && (authSize > 0));
        wprintf(L"  <LockoutAuth>%s</LockoutAuth>\n", (available) ? L"Available" : L"Unavailable");
    }
    else
    {
        wprintf(L"  <LockoutAuth>NotSet</LockoutAuth>\n");
    }
    if (((TPMA_PERMANENT*)&value)->endorsementAuthSet != 0)
    {
        UINT32 authSize = 0;
        BOOL available = ((Tbsi_Get_OwnerAuth(g_hTbs, TBS_OWNERAUTH_TYPE_ENDORSEMENT, nullptr, &authSize) == TBS_SUCCESS) && (authSize > 0));
        wprintf(L"  <EndorsementAuth>%s</EndorsementAuth>\n", (available) ? L"Available" : L"Unavailable");
    }
    else
    {
        wprintf(L"  <EndorsementAuth>NotSet</EndorsementAuth>\n");
    }
    if (((TPMA_PERMANENT*)&value)->ownerAuthSet != 0)
    {
        UINT32 authSize = 0;
        BOOL available = ((Tbsi_Get_OwnerAuth(g_hTbs, TBS_OWNERAUTH_TYPE_ADMIN, nullptr, &authSize) == TBS_SUCCESS) && (authSize > 0));
        wprintf(L"  <StorageAuth>%s</StorageAuth>\n", (available) ? L"Available" : L"Unavailable");
    }
    else
    {
        wprintf(L"  <StorageAuth>NotSet</StorageAuth>\n");
    }
    wprintf(L"  <DisableClear>%s</DisableClear>\n", (((TPMA_PERMANENT*)&value)->disableClear != 0) ? L"true" : L"false");
    wprintf(L"  <InLockout>%s</InLockout>\n", (((TPMA_PERMANENT*)&value)->inLockout) ? L"true" : L"false");

    // The ancient Intel TEE TPM on the MinnowBoardMax is Spec Rev 0.93 and is missing a lot of functionality.
    // Since RSA is mandatory for now and we don't anticipate RSA-less TPMs anytime soon, let's skip this test for now, but we'll leave the code here. *SIGH*
    wprintf(L"  <RSA>true</RSA>\n");
    /*
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_COMMANDS;
    tpm_in.getCapability.property = TPM_CC_RSA_Decrypt;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    if (tpm_out.getCapability.capabilityData.data.command.commandAttributes[0].commandIndex == TPM_CC_RSA_Decrypt)
    {
    INITIALIZE_CALL_BUFFERS(TPM2_TestParms, &tpm_in.testParms, &tpm_out.testParms);
    tpm_in.testParms.parameters.type = TPM_ALG_RSA;
    tpm_in.testParms.parameters.parameters.rsaDetail.keyBits = 2048;
    tpm_in.testParms.parameters.parameters.rsaDetail.scheme.scheme = TPM_ALG_RSASSA;
    tpm_in.testParms.parameters.parameters.rsaDetail.scheme.details.rsassa.hashAlg = TPM_ALG_SHA256;
    tpm_in.testParms.parameters.parameters.rsaDetail.symmetric.algorithm = TPM_ALG_NULL;
    tpm_in.testParms.parameters.parameters.rsaDetail.exponent = 0;
    cbCmd = TPM2_TestParms_Marshal(sessionTable, sessionCnt, &parms, &buffer, &size);
    if ((result = PlatformSubmitTPM20Command(FALSE, pbCmd, cbCmd, pbRsp, sizeof(pbRsp), &cbRsp)) != TPM_RC_SUCCESS)
    {
    throw result;
    }
    buffer = pbRsp;
    size = cbRsp;
    if ((result = TPM2_TestParms_Unmarshal(sessionTable, sessionCnt, &parms, &buffer, &size)) == TPM_RC_SUCCESS)
    {
    wprintf(L"  <RSA>true</RSA>\n");
    }
    else
    {
    wprintf(L"  <RSA>false</RSA>\n");
    }
    }
    else
    {
    wprintf(L"  <RSA>false</RSA>\n");
    }
    */

    // Detect HMAC support
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_COMMANDS;
    tpm_in.getCapability.property = TPM_CC_HMAC;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    if (tpm_out.getCapability.capabilityData.data.command.commandAttributes[0].commandIndex == TPM_CC_HMAC)
    {
        INITIALIZE_CALL_BUFFERS(TPM2_TestParms, &tpm_in.testParms, &tpm_out.testParms);
        tpm_in.testParms.parameters.type = TPM_ALG_KEYEDHASH;
        tpm_in.testParms.parameters.parameters.keyedHashDetail.scheme.scheme = TPM_ALG_HMAC;
        tpm_in.testParms.parameters.parameters.keyedHashDetail.scheme.details.hmac.hashAlg = TPM_ALG_SHA256;
        cbCmd = TPM2_TestParms_Marshal(sessionTable, sessionCnt, &parms, &buffer, &size);
        if ((result = PlatformSubmitTPM20Command(FALSE, pbCmd, cbCmd, pbRsp, sizeof(pbRsp), &cbRsp)) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        buffer = pbRsp;
        size = cbRsp;
        if ((result = TPM2_TestParms_Unmarshal(sessionTable, sessionCnt, &parms, &buffer, &size)) == TPM_RC_SUCCESS)
        {
            wprintf(L"  <HMAC>true</HMAC>\n");
        }
        else
        {
            wprintf(L"  <HMAC>false</HMAC>\n");
        }
    }
    else
    {
        wprintf(L"  <HMAC>false</HMAC>\n");
    }

    // Detect AES support
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_COMMANDS;
    tpm_in.getCapability.property = TPM_CC_EncryptDecrypt;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    if (tpm_out.getCapability.capabilityData.data.command.commandAttributes[0].commandIndex == TPM_CC_EncryptDecrypt)
    {
        INITIALIZE_CALL_BUFFERS(TPM2_TestParms, &tpm_in.testParms, &tpm_out.testParms);
        tpm_in.testParms.parameters.type = TPM_ALG_SYMCIPHER;
        tpm_in.testParms.parameters.parameters.symDetail.algorithm = TPM_ALG_AES;
        tpm_in.testParms.parameters.parameters.symDetail.keyBits.aes = MAX_AES_KEY_BITS;
        tpm_in.testParms.parameters.parameters.symDetail.mode.aes = TPM_ALG_CFB;
        cbCmd = TPM2_TestParms_Marshal(sessionTable, sessionCnt, &parms, &buffer, &size);
        if ((result = PlatformSubmitTPM20Command(FALSE, pbCmd, cbCmd, pbRsp, sizeof(pbRsp), &cbRsp)) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        buffer = pbRsp;
        size = cbRsp;
        if ((result = TPM2_TestParms_Unmarshal(sessionTable, sessionCnt, &parms, &buffer, &size)) == TPM_RC_SUCCESS)
        {
            wprintf(L"  <AES>true</AES>\n");
        }
        else
        {
            wprintf(L"  <AES>false</AES>\n");
        }
    }
    else
    {
        wprintf(L"  <AES>false</AES>\n");
    }

    // Detect ECC support
    INITIALIZE_CALL_BUFFERS(TPM2_GetCapability, &tpm_in.getCapability, &tpm_out.getCapability);
    tpm_in.getCapability.capability = TPM_CAP_COMMANDS;
    tpm_in.getCapability.property = TPM_CC_ECC_Parameters;
    tpm_in.getCapability.propertyCount = 1;
    LIMPET_TPM_CALL(FALSE, TPM2_GetCapability);
    if (tpm_out.getCapability.capabilityData.data.command.commandAttributes[0].commandIndex == TPM_CC_ECC_Parameters)
    {
        INITIALIZE_CALL_BUFFERS(TPM2_TestParms, &tpm_in.testParms, &tpm_out.testParms);
        tpm_in.testParms.parameters.type = TPM_ALG_ECC;
        tpm_in.testParms.parameters.parameters.eccDetail.curveID = TPM_ECC_NIST_P256;
        tpm_in.testParms.parameters.parameters.eccDetail.scheme.details.ecdsa.hashAlg = TPM_ALG_SHA256;
        tpm_in.testParms.parameters.parameters.eccDetail.scheme.scheme = TPM_ALG_ECDSA;
        tpm_in.testParms.parameters.parameters.eccDetail.symmetric.algorithm = TPM_ALG_NULL;
        tpm_in.testParms.parameters.parameters.eccDetail.kdf.scheme = TPM_ALG_NULL;
        cbCmd = TPM2_TestParms_Marshal(sessionTable, sessionCnt, &parms, &buffer, &size);
        if ((result = PlatformSubmitTPM20Command(FALSE, pbCmd, cbCmd, pbRsp, sizeof(pbRsp), &cbRsp)) != TPM_RC_SUCCESS)
        {
            throw result;
        }
        buffer = pbRsp;
        size = cbRsp;
        if ((result = TPM2_TestParms_Unmarshal(sessionTable, sessionCnt, &parms, &buffer, &size)) == TPM_RC_SUCCESS)
        {
            wprintf(L"  <ECC>true</ECC>\n");
        }
        else
        {
            wprintf(L"  <ECC>false</ECC>\n");
        }
    }
    else
    {
        wprintf(L"  <ECC>false</ECC>\n");
    }

    // Platform integration is indicated if we have a TCG log
    UINT32 logSize = 0;
    if (Tbsi_Get_TCG_Log(g_hTbs, NULL, &logSize) != TBS_SUCCESS)
    {
        logSize = 0;
    }
    wprintf(L"  <TcgLogSize>%d</TcgLogSize>\n", logSize);

    wprintf(L"</TpmInfo>\n");
}

// Just dump all ACPITABL.DATs we have on screen
static void EnumerateACPITABLs(
    void
)
{
    UINT32 n = 0;
    wprintf(L"<TpmAddInSupport>\n");
    wprintf(L"  <Generic>\n");
    for (; n < 2; n++)
    {
        wprintf(L"    <ACPITABL index=\"%d\">%s</ACPITABL>\n", n, acpitableName[n]);
    }
    wprintf(L"  </Generic>\n  <RPi>\n");
    for (; n < 8; n++)
    {
        wprintf(L"    <ACPITABL index=\"%d\">%s</ACPITABL>\n", n, acpitableName[n]);
    }
    wprintf(L"  </RPi>\n  <MBM>\n");
    for (; n < NUM_ACPITABL; n++)
    {
        wprintf(L"    <ACPITABL index=\"%d\">%s</ACPITABL>\n", n, acpitableName[n]);
    }
    wprintf(L"  </MBM>\n");
    wprintf(L"</TpmAddInSupport>\n");
}

// Write the selected one out to the file system or delete it
static void InstallTpmACPITABL(
    __in DWORD index
)
{
    WCHAR acpiTablFile[LIMPET_STRING_SIZE] = L"";

    if ((GetSystemDirectoryW(acpiTablFile, LIMPET_STRING_SIZE) == 0) ||
        (FAILED(StringCchCatW(acpiTablFile, LIMPET_STRING_SIZE, L"\\ACPITABL.DAT"))))
    {
        throw TPM_RC_FAILURE;
    }

    if (acpitablSize[index] == 0)
    {
        // Best effort delete of the current ACPITABL.DAT
        DeleteFileW(acpiTablFile);
    }
    else
    {
        // Create or overwrite the ACPITABL.DAT in c:\windows\system32\ACPITABL.DAT
        std::vector<BYTE> selectedTabl(acpitablSize[index]);
        memcpy(&selectedTabl[0], acpitabl[index], selectedTabl.size());
        std::wstring acpiTableFileName(acpiTablFile);
        WriteToDisk(acpiTableFileName, selectedTabl);
    }
}

static void GetPlatformKey2BPub(
    DWORD keyHandle,
    __in_ecount_z(LIMPET_STRING_SIZE) WCHAR* szDataFile
)
{
    UINT32 result = TPM_RC_SUCCESS;

    // Get the public key blob
    std::vector<BYTE> keyBuffer(1024);
    UINT32 cbKeyBuffer = (UINT32)keyBuffer.size();
    if ((result = LimpetGetTpmKey2BPub(keyHandle, &keyBuffer[0], (UINT32)keyBuffer.size(), &cbKeyBuffer, NULL)) != TPM_RC_SUCCESS)
    {
        throw result;
    }
    keyBuffer.resize(cbKeyBuffer);

    // Print the base64 encoded buffer
    std::wstring wcEncodedBuffer = MakePrintable(keyBuffer, CRYPT_STRING_BASE64);
    wprintf(L"%s\n", &wcEncodedBuffer[0]);

    // Write to the specificd file if requested
    if (szDataFile != nullptr)
    {
        std::wstring dataFileName(szDataFile);
        WriteToDisk(dataFileName, keyBuffer);
    }
}

// If fullHelp == false, we just show a subset of all the available commands (the rest is not documented)
static void GetHelp(
    bool fullHelp = false
)
{
    wprintf((fullHelp ? GLOBAL_COMMAND_HELP_ALL : GLOBAL_COMMAND_HELP_PUBLIC));
    if (TpmFound)
    {
        wprintf((fullHelp ? TPMAVAILABLE_COMMAND_HELP_ALL : TPMAVAILABLE_COMMAND_HELP_PUBLIC));
    }
}

HRESULT InvalidCommandLineUsage()
{
    wprintf(L"\r\nError: unrecognized or incomplete command line.\r\n");
    GetHelp();
    return E_INVALIDARG;
}

bool IsSwitchActive(const std::wstring& switchSel)
{
    const size_t size = g_parametersLC.size();
    for (size_t i = 0; i < size; i++)
    {
        if (((g_parametersLC[i].compare(0, 1, L"/") == 0) ||
            (g_parametersLC[i].compare(0, 1, L"-") == 0)) &&
            (g_parametersLC[i].compare(1, std::wstring::npos, switchSel) == 0))
        {
            return true;
        }
    }

    return false;
}

// It searches switch in the parameters and returns the index.
// If not found, it returns the size of the index.
size_t GetSwitchIndex(const std::wstring& switchSel)
{
    std::wstring token{ L"-" + switchSel };
    auto result = std::find(g_parametersLC.begin(), g_parametersLC.end(), token);
    size_t tokenIndex = std::distance(g_parametersLC.begin(), result);
    if (tokenIndex == g_parametersLC.size())
    {
        token = L"/" + switchSel;
        result = std::find(g_parametersLC.begin(), g_parametersLC.end(), token);
        tokenIndex = std::distance(g_parametersLC.begin(), result);
    }
    return tokenIndex;
}

const std::wstring GetSwitchValue(const std::vector<std::wstring>& parameters, const std::wstring& switchSel)
{
    auto switchSelIndex = GetSwitchIndex(switchSel);

    if (switchSelIndex + 1 >= parameters.size())
    {
        return L"";
    }

    return parameters[switchSelIndex + 1];
}


HRESULT ProcessAzureDpsRegisterCommand(const std::vector<std::wstring>& parameters)
{
    HRESULT hr = S_OK;

    const auto switchIndex = GetSwitchIndex(PARAMETER_COMMAND_REGISTER);

    if (parameters.size() <= switchIndex + 3)
    {
        return InvalidCommandLineUsage();
    }

    const int tpmSlotNumber = _wtoi(parameters[switchIndex + 1].c_str());
    const std::wstring globalDpsServiceUri = parameters[switchIndex + 2];
    const std::wstring scopeId = parameters[switchIndex + 3];

    hr = AzureDpsRegisterDeviceWithoutModules(tpmSlotNumber, globalDpsServiceUri, scopeId);

    if (hr == S_OK)
    {
        wprintf(L"device registration completed succesfully in Azure dps.\r\n");
    }
    else
    {
        wprintf(L"device registration in Azure dps failed hr = 0x%x\r\n", hr);
    }

    return hr;
}


HRESULT ProcessAzureDpsEnrollmentInfoCommand()
{
    HRESULT hr = S_OK;
    bool fCSV = false;
    bool fJSON = false;

    std::vector<BYTE> localEkPub(LIMPET_STRING_SIZE);
    UINT32 cblocalEkPub = LIMPET_STRING_SIZE;
    hr = GetEndorsementKeyPub(&localEkPub[0], &cblocalEkPub);
    if (FAILED(hr)) {
        wprintf(L"Can't get endorsement key. hr = %x\r\n", hr);
        return hr;
    }
    localEkPub.resize(cblocalEkPub);
    const std::wstring ek64{ MakePrintable(localEkPub, CRYPT_STRING_BASE64) };

    // current azure docs(2017-08-21 preview) for enrollment REST api put the storage root key in tpm attestation structure 
    // this appears to be an artifact of their doc generating process since the tpm type for registration(as opposed to enrollment) 
    // does need the srk property.  there's no way to provide the srk in the enrollment gui and the azure sdk version of
    // the factory tool doesn't output that information
    std::wstring registrationId(LIMPET_STRING_SIZE, L'\0');
    size_t cchregistrationId = LIMPET_STRING_SIZE;
    hr = GetRegistrationId(&registrationId[0], &cchregistrationId);
    if (FAILED(hr)) {
        wprintf(L"Can't get registration Id. hr = %x\r\n", hr);
        return hr;
    }

    if (IsSwitchActive(PARAMETER_SWITCH_FORMAT_CSV))
    {
        fCSV = true;
        wprintf(L"\"Registration Id\", \"Endorsement Key\"\r\n");
        wprintf(L"\"%s\", \"%s\"\r\n", registrationId.c_str(), ek64.c_str());
    }

    if (IsSwitchActive(PARAMETER_SWITCH_FORMAT_JSON))
    {
        fJSON = true;
        wprintf(L"[\r\n");
        wprintf(L"{\r\n");
        wprintf(L"\t\"attestation\": {\r\n");
        wprintf(L"\t\t\"type\": \"tpm\",\r\n");
        wprintf(L"\t\t\"tpm\": {\r\n");
        wprintf(L"\t\t\t\"endorsementKey\": \"%s\"", ek64.c_str());
        wprintf(L"\r\n");
        wprintf(L"\t\t}\r\n"); // tpm
        wprintf(L"\t},\r\n"); // attestation
        wprintf(L"\t\"registrationId\": \"%s\"\r\n", registrationId.c_str());
        wprintf(L"}\r\n]\r\n");
    }

    if (IsSwitchActive(PARAMETER_SWITCH_FORMAT_TXT) ||
        (!fCSV && !fJSON))
    {
        wprintf(L"\r\nProvisioning information\r\n\r\n");
        wprintf(L"\tRegistration Id:\r\n");
        wprintf(L"\t%s\r\n\r\n", registrationId.c_str());
        wprintf(L"\tEndorsement Key:\r\n");
        wprintf(L"\t%s\r\n\r\n", ek64.c_str());
    }
    return hr;
}

// SHA256-HMAC test vector
// Encoded key:  bZD+qa+2VxMAX7yi6Ar2ldJ90tEkBjlgVsN74Ptz9ac=
// Key:          6d90fea9afb65713005fbca2e80af695d27dd2d12406396056c37be0fb73f5a7
// Data:         6162636465666768696A6B6C6D6E6F707172737475767778797A200D0A
// SHA256-HMAC:  20B2990C1659F5A40BAF75C5B308E012E4ECF8DD8ED80D49B5DB811705D67847
// Encoded HMAC: ILKZDBZZ9aQLr3XFswjgEuTs+N2O2A1JtduBFwXWeEc=
//
// limpet 0 -chk bZD+qa+2VxMAX7yi6Ar2ldJ90tEkBjlgVsN74Ptz9ac=
// echo abcdefghijklmnopqrstuvwxyz >vector.txt
// limpet 0 -shk vector.txt

UINT32
__cdecl
__cdecl
wmain(
    __in INT32 argc,
    __in_ecount(argc) LPCWSTR argv[]
)
{
    TBS_RESULT result = TBS_SUCCESS;
    UINT32 logicalDeviceNumber = 0;

    // No parameter, no action
    if (argc <= 1)
    {
        GetHelp();
        return TPM_RC_SUCCESS;
    }

    // Put the parameters in a nice table
    std::wstring command(argv[1]);
    WStringToLower(command);
    std::vector<std::wstring> parameters;
    for (INT32 n = 2; n < argc; n++)
    {
        parameters.push_back(std::wstring(argv[n]));
        std::wstring parameter{ argv[n] };
        g_parametersLC.push_back(WStringToLower(parameter));
    }

    // INTERNAL: See if we are supposed to break into the debugger
    if (command == std::wstring(PARAMETER_BREAK_ON_ENTER))
    {
        // Drop the first parameter
        if (parameters.empty())
        {
            GetHelp();
            return TPM_RC_SUCCESS;
        }
        command = WStringToLower(parameters[0]);
        parameters.erase(parameters.begin());
        __debugbreak();
    }

    // INTERNAL: See if we need to display the full help
    if (command == std::wstring(PARAMETER_FULL_HELP))
    {
        GetHelp(true);
        return TPM_RC_SUCCESS;
    }

    LimpetPlatformInitialize lpi;

    // TPM independent commands
    if ((command == std::wstring(PARAMETER_LIMPET_VERSION)) &&
        (parameters.empty())) // No parameters
    {
        wprintf(XML_HEADER);
        wprintf(L"<Version>%u</Version>\n", LIMPET_VERSION);
        return result;
    }
    else if ((command == std::wstring(PARAMETER_LIST_ADDON_TPMS)) &&
        (parameters.empty())) // No parameters
    {
        wprintf(XML_HEADER);
        EnumerateACPITABLs();
        return result;
    }
    else if ((command == std::wstring(PARAMETER_INSTALL_ADDON_TPM)) &&
        (parameters.size() == 1))
    {
        DWORD index = 0;

        wprintf(XML_HEADER);
        try
        {
            if (swscanf_s((parameters[0]).c_str(), L"%d", &index) != 1)
            {
                throw TPM_RC_FAILURE;
            }
            InstallTpmACPITABL(index);
            wprintf(L"<Installed>%s</Installed>\n", acpitableName[index]);
        }
        catch (...)
        {
            wprintf(L"<Installed/>\n");
            result = TPM_RC_NO_RESULT;
        }
        return result;
    }
    else if ((command == std::wstring(PARAMETER_ENCRYPT_PAYLOAD)) &&
        (parameters.size() == 2))
    {
        std::wstring& payloadKey = parameters[0];
        std::wstring& payloadFile = parameters[1];

        wprintf(XML_HEADER);
        try
        {
            std::vector<BYTE> aesKey;
            try
            {
                // See if the payload key is a valid file name and read the data if it is
                aesKey = ReadFromDisk(payloadKey);
            }
            catch (...)
            {
                // Since it is not a file we assume it is the actual base64 encoded key itself
                aesKey = ReadBinary(payloadKey, CRYPT_STRING_BASE64);
            }
            if (aesKey.size() != MAX_AES_KEY_BYTES)
            {
                throw TPM_RC_FAILURE;
            }

            if ((result = LimpetSymmetricPayloadProtection(true, &aesKey[0], (WCHAR*)payloadFile.c_str())) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            wprintf(L"<Encrypted>\n  %s\n</Encrypted>\n", payloadFile.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<Encrypted/>\n");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<Encrypted/>\n");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_DECRYPT_PAYLOAD)) &&
        (parameters.size() == 2))
    {
        std::wstring& payloadKey = parameters[0];
        std::wstring& payloadFile = parameters[1];

        wprintf(XML_HEADER);
        try
        {
            std::vector<BYTE> aesKey;
            try
            {
                // See if the payload key is a valid file name and read the data if it is
                aesKey = ReadFromDisk(payloadKey);
            }
            catch (...)
            {
                // Since it is not a file we assume it is the actual base64 encoded key itself
                aesKey = ReadBinary(payloadKey, CRYPT_STRING_BASE64);
            }
            if (aesKey.size() != MAX_AES_KEY_BYTES)
            {
                throw TPM_RC_FAILURE;
            }

            if ((result = LimpetSymmetricPayloadProtection(false, &aesKey[0], (WCHAR*)payloadFile.c_str())) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            wprintf(L"<Decrypted>\n  %s\n</Decrypted>\n", payloadFile.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<Decrypted/>\n");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<Decrypted/>\n");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_SIGN_WITH_CERT)) &&
        (parameters.size() == 2))
    {
        std::wstring& certFileName = parameters[0];
        std::wstring& payloadFile = parameters[1];

        wprintf(XML_HEADER);
        try
        {
            // Cacluate the cert thumbprint so it can be looked up in the certificate store
            AutoCloseCertificateContext signingCert = ReadEncodedCert(certFileName);
            std::vector<BYTE> signingCertThumbprint(SHA1_DIGEST_SIZE, 0);
            DWORD cbCertThumbprint = (DWORD)signingCertThumbprint.size();
            if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM,
                0,
                NULL,
                signingCert.Get()->pbCertEncoded,
                signingCert.Get()->cbCertEncoded,
                &signingCertThumbprint[0],
                &cbCertThumbprint))
            {
                throw TPM_RC_FAILURE;
            }

            // Read the data file
            std::vector<BYTE> dataToSign = ReadFromDisk(payloadFile);

            // Get the job done
            std::wstring subjectName(LIMPET_STRING_SIZE, L'\0');
            std::vector<BYTE> signature(1024);
            UINT32 cbSignature;
            if ((result = LimpetSignWithIdentityCert(&signingCertThumbprint[0],
                &dataToSign[0],
                (DWORD)dataToSign.size(),
                (WCHAR*)subjectName.c_str(),
                &signature[0],
                (UINT32)signature.size(),
                &cbSignature)) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            subjectName.resize(subjectName.find(L'\0'));
            signature.resize(cbSignature);

            // Output the results
            std::wstring printableSignature = MakePrintable(signature, CRYPT_STRING_BASE64);
            wprintf(L"<Signed>\n  <Subject>\n    %s\n  </Subject>\n  <Signature>\n    %s\n  </Signature>\n</Signed>", &subjectName[0], &printableSignature[0]);
        }
        catch (UINT32 err)
        {
            wprintf(L"<Signed/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<Signed/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_IMPORT_AUTH_CERT)) &&
        (parameters.size() == 1))
    {
        std::wstring& certFileName = parameters[0];

        wprintf(XML_HEADER);
        try
        {
            // Read the cert file
            std::vector<BYTE> importCert = ReadFromDisk(certFileName);

            // Add the new certificate
            std::vector<BYTE> certThumbPrint(SHA1_DIGEST_SIZE);
            std::wstring keyName(LIMPET_STRING_SIZE, L'\0');
            if ((result = LimpetImportIdentityCert(&importCert[0],
                (UINT32)importCert.size(),
                &certThumbPrint[0],
                (WCHAR*)keyName.c_str())) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            keyName.resize(keyName.find(L'\0'));

            std::wstring printableImportCert = MakePrintable(importCert, CRYPT_STRING_BASE64);
            std::wstring printableCertThumbPrint = MakePrintable(certThumbPrint, CRYPT_STRING_HEXRAW);
            wprintf(L"<IDKCert>\n  <KeyName>\n    %s\n  </KeyName>\n  <ThumbPrint>\n    %s\n  </ThumbPrint>\n  <Cert>\n    %s\n  </Cert>\n</IDKCert>", keyName.c_str(), printableCertThumbPrint.c_str(), printableImportCert.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<IDKCert/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<IDKCert/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_AUTH_ISSUE_CERT)) &&
        (parameters.size() == 4))
    {
        std::wstring& caCertFileName = parameters[0];
        std::wstring& certFileName = parameters[1];
        std::wstring& assignedDeviceId = parameters[2];
        std::wstring& assignedUrl = parameters[3];

        wprintf(XML_HEADER);
        try
        {
            // Cacluate the CA signing cert thumbprint so it can be looked up in the certificate store
            AutoCloseCertificateContext caSigningCert = ReadEncodedCert(caCertFileName);
            std::vector<BYTE> caSigningCertThumbprint(SHA1_DIGEST_SIZE, 0);
            DWORD cbCaSigningCertThumbprint = (DWORD)caSigningCertThumbprint.size();
            if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM,
                0,
                NULL,
                caSigningCert.Get()->pbCertEncoded,
                caSigningCert.Get()->cbCertEncoded,
                &caSigningCertThumbprint[0],
                &cbCaSigningCertThumbprint))
            {
                throw TPM_RC_FAILURE;
            }

            // Read the cert file
            std::vector<BYTE> selfSignedCert = ReadFromDisk(certFileName);

            // Get the job done
            std::vector<BYTE> tpmKeyName(LIMPET_TPM_OBJECT_NAME);
            std::vector<BYTE> issuedCertThumbprint(SHA1_DIGEST_SIZE);
            std::vector<BYTE> issuedCert(4096);
            UINT32 cbIssuedCert;
            if ((result = LimpetIssueCertificate(&caSigningCertThumbprint[0],
                &selfSignedCert[0],
                (UINT32)selfSignedCert.size(),
                (WCHAR*)assignedDeviceId.c_str(),
                (WCHAR*)assignedUrl.c_str(),
                &tpmKeyName[0],
                &issuedCertThumbprint[0],
                &issuedCert[0],
                (UINT32)issuedCert.size(),
                &cbIssuedCert)) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            issuedCert.resize(cbIssuedCert);

            // Overwrite the selfsigned cert with the issued cert
            WriteToDisk(certFileName, issuedCert, TRUNCATE_EXISTING);

            // Prepare the output
            std::wstring printableCertThumbPrint = MakePrintable(issuedCertThumbprint, CRYPT_STRING_HEXRAW);
            std::wstring printableTpmKeyName = MakePrintable(tpmKeyName, CRYPT_STRING_BASE64);
            std::wstring printableCert = MakePrintable(issuedCert, CRYPT_STRING_BASE64);
            wprintf(L"<IDKCert>\n  <TpmKeyName>\n    %s\n  </TpmKeyName>\n  <ThumbPrint>\n    %s\n  </ThumbPrint>\n  <Cert>\n    %s\n  </Cert>\n</IDKCert>", printableTpmKeyName.c_str(), printableCertThumbPrint.c_str(), printableCert.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<IDKCert/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<IDKCert/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_CHALLENGE_ASYM_ID)) &&
        (parameters.size() >= 3) && (parameters.size() < 6))
    {
        std::wstring& rekPubFileName = parameters[0];
        std::wstring& idkCertName = parameters[1];
        std::wstring activationFileName = parameters[2];
        std::wstring payloadFileName((parameters.size() > 3) ? parameters[3] : std::wstring(L""));
        std::wstring payloadKeyFileName((parameters.size() > 4) ? parameters[4] : std::wstring(L""));

        wprintf(XML_HEADER);
        try
        {
            // Read the REK file
            std::vector<BYTE> rekPubTpm2b = ReadFromDisk(rekPubFileName);

            // Harvest the IDKName
            std::vector<BYTE> idkCert = ReadFromDisk(idkCertName);

            // Get the IDK TPM name
            std::vector<BYTE> idkTpmName(LIMPET_TPM_OBJECT_NAME);
            UINT32 cbKeyPub;
            if ((result = LimpetGetTpmKey2BPubFromCert(&idkCert[0],
                (UINT32)idkCert.size(),
                nullptr,
                0,
                &cbKeyPub,
                &idkTpmName[0])) != TPM_RC_SUCCESS)
            {
                throw result;
            }

            // Create the activation
            std::vector<BYTE> activationSecret(MAX_AES_KEY_BYTES);
            std::vector<BYTE> activationBlob(2048);
            UINT32 cbActivationBlob;
            if ((result = LimpetChallengeIdentityCert(&rekPubTpm2b[0],
                (UINT32)rekPubTpm2b.size(),
                &idkTpmName[0],
                &activationSecret[0],
                &activationBlob[0],
                (UINT32)activationBlob.size(),
                &cbActivationBlob)) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            activationBlob.resize(cbActivationBlob);

            // Write the activation
            WriteToDisk(activationFileName, activationBlob);

            // Encrypt the payload if requested
            if (payloadFileName.size() > 0)
            {
                if ((result = LimpetSymmetricPayloadProtection(true, &activationSecret[0], (WCHAR*)payloadFileName.c_str())) != TPM_RC_SUCCESS)
                {
                    throw result;
                }
            }

            // Write the payload key to a file if requested
            if (payloadKeyFileName.size() > 0)
            {
                WriteToDisk(payloadKeyFileName, activationSecret);
            }

            // Prepare the output
            std::wstring printableActivationSecret = MakePrintable(activationSecret, CRYPT_STRING_BASE64);
            std::wstring printableActivationBlob = MakePrintable(activationBlob, CRYPT_STRING_BASE64);
            wprintf(L"<Activation>\n  <PayloadKey>\n    %s\n  </PayloadKey>\n  <Blob>\n    %s\n  </Blob>\n</Activation>", printableActivationSecret.c_str(), printableActivationBlob.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<Activation/>\n");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<Activation/>\n");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_CREATE_BOUND_SYM_ID)) &&
        (parameters.size() >= 6) && (parameters.size() < 8))
    {
        std::wstring& rekPubFileName = parameters[0];
        std::wstring& srkPubFileName = parameters[1];
        std::wstring& printableHmacKey = parameters[2];
        std::wstring& deviceId = parameters[3];
        std::wstring& serviceUrl = parameters[4];
        std::wstring& importBlobFileName = parameters[5];
        std::wstring payloadKeyFileName((parameters.size() > 6) ? parameters[6] : std::wstring(L""));

        wprintf(XML_HEADER);
        try
        {
            // Read the REK file
            std::vector<BYTE> rekPubTpm2b = ReadFromDisk(rekPubFileName);

            // Read the SRK file
            std::vector<BYTE> srkPubTpm2b = ReadFromDisk(srkPubFileName);

            // Harvest the hmacKey
            std::vector<BYTE> hmacKey = ReadBinary(printableHmacKey, CRYPT_STRING_BASE64);
            if ((hmacKey.size() < SHA256_DIGEST_SIZE / 2) ||
                (hmacKey.size() > SHA256_DIGEST_SIZE * 2))
            {
                throw TPM_RC_FAILURE;
            }

            //Create the import blob
            std::vector<BYTE> activationSecret(MAX_AES_KEY_BYTES);
            std::vector<BYTE> importBlob(2048);
            UINT32 cbImportBlob;
            if ((result = LimpetCreateHostageKeyEx(&rekPubTpm2b[0],
                (UINT32)rekPubTpm2b.size(),
                &srkPubTpm2b[0],
                (UINT32)srkPubTpm2b.size(),
                (WCHAR*)deviceId.c_str(),
                (WCHAR*)serviceUrl.c_str(),
                &hmacKey[0],
                (UINT32)hmacKey.size(),
                &activationSecret[0],
                &importBlob[0],
                (UINT32)importBlob.size(),
                &cbImportBlob)) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            importBlob.resize(cbImportBlob);

            // Write the import blob
            WriteToDisk(importBlobFileName, importBlob);

            // Write the payload key to a file if requested
            if (payloadKeyFileName.size() > 0)
            {
                WriteToDisk(payloadKeyFileName, activationSecret);
            }

            // Prepare the output
            std::wstring printableActivationSecret = MakePrintable(activationSecret, CRYPT_STRING_BASE64);
            std::wstring printableImportBlob = MakePrintable(importBlob, CRYPT_STRING_BASE64);
            wprintf(L"<KeyImport>\n  <PayloadKey>\n    %s\n  </PayloadKey>\n  <Blob>\n    %s\n  </Blob>\n</KeyImport>", printableActivationSecret.c_str(), printableImportBlob.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<KeyImport/>\n");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<KeyImport/>\n");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_DUMP_CERTSTORE)) &&
        (parameters.size() == 1))
    {
        std::wstring& storeName = parameters[0];

        wprintf(XML_HEADER);
        wprintf(L"<CertStore store=\"%s\">\n", &storeName[0]);
        try
        {
            // Open the requested store
            AutoCloseHCertStore store(CertOpenStore(CERT_STORE_PROV_SYSTEM, PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, NULL, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, &storeName[0]));
            if (store.Get() == NULL)
            {
                throw TPM_RC_FAILURE;
            }

            // Enumerate all certs in the store
            PCCERT_CONTEXT nextCert = NULL;
            UINT32 index = 0;
            for (;;)
            {
                // this function closes all the certificate context, no AutoClose type needed here.
                nextCert = CertEnumCertificatesInStore(store.Get(), nextCert);
                if (nextCert == NULL)
                {
                    break;
                }

                // Show the entry
                wprintf(L"  <Cert Index=\"%u\">\n", index++);
                DumpCertInfo(std::wstring(L"    "), nextCert);
                wprintf(L"  </Cert>\n");
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
        wprintf(L"</CertStore>\n");

        return result;
    }
    else if ((command == std::wstring(PARAMETER_IMPORT_CERTIFICATE)) &&
        (parameters.size() == 2))
    {
        std::wstring& certFileName = parameters[0];
        std::wstring& storeName = parameters[1];

        wprintf(XML_HEADER);
        wprintf(L"<CertImport>\n");
        try
        {
            // Read the cert file and open it
            AutoCloseCertificateContext newCert = ReadEncodedCert(certFileName);

            // Open the requested store
            AutoCloseHCertStore store(CertOpenStore(CERT_STORE_PROV_SYSTEM, PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, NULL, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, &storeName[0]));
            if (store.Get() == NULL)
            {
                throw TPM_RC_FAILURE;
            }

            // Add the certificate to the store
            PCCERT_CONTEXT newCertInStore = NULL;
            if (!CertAddCertificateContextToStore(store.Get(), newCert.Get(), CERT_STORE_ADD_ALWAYS, &newCertInStore))
            {
                throw TPM_RC_FAILURE;
            }

            wprintf(L"  <Store>\n    %s\n  </Store>\n", storeName.c_str());
            DumpCertInfo(std::wstring(L"  "), newCertInStore);
        }
        catch (UINT32 err)
        {
            result = err;
        }
        catch (...)
        {
            result = TPM_RC_FAILURE;
        }
        wprintf(L"</CertImport>\n");

        return result;
    }

    // We are done with the commands that have no dependency on the TPM.
    // For anything that happens past this point we have to have a TPM.

    // If there is a TPM, do we like it?
    if ((Tbsi_GetDeviceInfo(sizeof(deviceInfo), &deviceInfo) != TBS_SUCCESS) ||
        (deviceInfo.structVersion != 1) ||
        (deviceInfo.tpmVersion != TPM_VERSION_20) ||
        (Tbsi_Context_Create((PCTBS_CONTEXT_PARAMS)&context, &g_hTbs) != TBS_SUCCESS))
    {
        // Yuk! Bad or no TPM. Bail.
        wprintf(XML_HEADER);
        wprintf(L"<TpmInfo>TpmNotSupported</TpmInfo>\n");
        return TPM_RC_FAILURE;
    }

    // Next we are looking at commands without logical device identity
    if ((command == std::wstring(PARAMETER_GET_TPM_INFO)) &&
        (parameters.empty()))
    {
        wprintf(XML_HEADER);
        GetTpmInfo();
        return TPM_RC_SUCCESS;
    }
    else if ((command == std::wstring(PARAMETER_FORCE_CLEAR_TPM)) &&
        (parameters.empty()))
    {
        wprintf(XML_HEADER);
        try
        {
            // Lobotomize the TPM and loose everything - I mean everything. That includes the BitLocker
            // Device Encryption key if it is enabled and turn the device into a spiffy little brick
            // on the next reboot. Trust me I've been there. Yikes!
            LimpetForceClearTpm();
            wprintf(L"<TpmClear>  OK\n</TpmClear>\n");
        }
        catch (...)
        {
            wprintf(L"<TpmClear/>\n");
            result = TPM_RC_NO_RESULT;
        }
        return result;
    }
    else if ((command == std::wstring(PARAMETER_LIST_LOGICAL_DEVICES)) &&
        (parameters.empty()))
    {
        wprintf(XML_HEADER);
        wprintf(L"<LogicalDevices>\n");
        try
        {
            ListLogicalDevices();
        }
        catch (...)
        {
            result = TPM_RC_NO_RESULT;
        }
        wprintf(L"</LogicalDevices>\n");
        return result;
    }
    else if ((command == std::wstring(PARAMETER_GET_SRK)) &&
        (parameters.size() < 2))
    {
        std::wstring srkPubFileName{ parameters.empty() ? std::wstring() : parameters[0] };

        wprintf(XML_HEADER);
        wprintf(L"<SRKPub>\n");
        try
        {
            GetPlatformKey2BPub(LIMPET_TPM20_SRK_HANDLE, (srkPubFileName.size() != 0) ? (WCHAR*)srkPubFileName.c_str() : nullptr);
        }
        catch (...)
        {
            result = TPM_RC_NO_RESULT;
        }
        wprintf(L"</SRKPub>\n");
        return result;
    }
    else if ((command == std::wstring(PARAMETER_GET_ERK))
        && (parameters.size() < 2))
    {
        std::wstring erkPubFileName{ parameters.empty() ? std::wstring() : parameters[0] };

        wprintf(XML_HEADER);
        wprintf(L"<ERKPub>\n");
        try
        {
            GetPlatformKey2BPub(LIMPET_TPM20_ERK_HANDLE, (erkPubFileName.size() != 0) ? (WCHAR*)erkPubFileName.c_str() : nullptr);
        }
        catch (...)
        {
            result = TPM_RC_NO_RESULT;
        }
        wprintf(L"</ERKPub>\n");
        return result;
    }
    else if ((command == std::wstring(PARAMETER_PFX_IMPORT)) &&
        (parameters.size() == 2))
    {
        std::wstring pfxFileName(parameters[0]);
        std::wstring pfxPassword(parameters[1]);

        HMODULE hModule = NULL;
        FNIMPORTPFXTOPROVIDERFREEDATA *pfnFree = NULL;
        FNIMPORTPFXTOPROVIDER *pfnImport = NULL;
        DWORD numCerts = 0;
        PCCERT_CONTEXT* certTableOut = NULL;

        wprintf(XML_HEADER);
        wprintf(L"<Cert>\n");
        try
        {
            // Load the pfx data from the file
            std::vector<BYTE> pfxData = ReadFromDisk(pfxFileName);

            // Crack open the PFX, import all included keys into PCPKSP and create the cert table

            PCWSTR pwszModule = L"CertEnroll.dll";
            PCSTR pszProc = "ImportPFXToProvider";
            hModule = LoadLibraryEx(pwszModule, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);

            if (NULL == hModule)
            {
                wprintf(L"error occurred in loading module %s.\r\n", pwszModule);
                throw TPM_RC_FAILURE;
            }

            pfnImport = (FNIMPORTPFXTOPROVIDER *)GetProcAddress(hModule, pszProc);

            if (NULL == pfnImport)
            {
                printf("error occurred in retrieving the exported function %s\r\n", pszProc);
                throw TPM_RC_FAILURE;
            }

            HRESULT hr = (*pfnImport)(NULL,
                &pfxData[0],
                (DWORD)pfxData.size(),
                (ImportPFXFlags)((DWORD)ImportForceOverwrite +
                (DWORD)ImportSilent +
                    (DWORD)ImportMachineContext +
                    (DWORD)ImportInstallCertificate +
                    (DWORD)ImportInstallChainAndRoot),
                &pfxPassword[0],
                MS_PLATFORM_KEY_STORAGE_PROVIDER,
                NULL,
                NULL,
                NULL,
                NULL,
                &numCerts,
                &certTableOut);

            if(FAILED(hr))
            {
                wprintf(L"error 0x%x occurred in crack opening the pfx certificate.\r\n", hr);
                throw TPM_RC_FAILURE;
            }

            // Process the returned certificate table
            for (DWORD n = 0; n < numCerts; n++)
            {
                AutoCloseCertificateContext context;
                context = certTableOut[n];
                DumpCertInfo(std::wstring(L"  "), context.Get());
            }

        }
        catch (const UINT32 err)
        { 
            result = err;
        }
        catch (...)
        {
            result = TPM_RC_NO_RESULT;
        }
        wprintf(L"</Cert>\n");

        if (NULL != pfnFree)
        {
            (*pfnFree)(numCerts, certTableOut);
        }

        if (NULL != hModule)
        {
            FreeLibrary(hModule);
        }

        return result;
    }

    else if (command == std::wstring(PARAMETER_AZUREDPS))
    {
        HRESULT hr = S_OK;
        g_parametersLC.insert(g_parametersLC.begin(), command);
        parameters.insert(parameters.begin(), command);


        if (IsSwitchActive(PARAMETER_COMMAND_ENROLLMENTINFO))
        {
            hr = ProcessAzureDpsEnrollmentInfoCommand();
        }
        else if (IsSwitchActive(PARAMETER_COMMAND_REGISTER))
        {
            hr = ProcessAzureDpsRegisterCommand(parameters);
        }
        else
        {
            hr = InvalidCommandLineUsage();
        }

        if (FAILED(hr))
        {
            wprintf(L"unexpected error occurred, please investigate");
        }
        return hr;
    }

    // All other commands require a logical device number first
    if ((swscanf_s(command.c_str(), L"%d", &logicalDeviceNumber) != 1) ||
        (logicalDeviceNumber > 9))
    {
        GetHelp();
        return result;
    }
    else
    {
        // Move the real command from the top of the parameter list over
        command = WStringToLower(parameters[0]);
        parameters.erase(parameters.begin());
    }

    // Parse the command for the
    if ((command == std::wstring(PARAMETER_READ_DEVICEID)) &&
        (parameters.empty()))
    {
        wprintf(XML_HEADER);
        try
        {
            std::wstring output(LIMPET_STRING_SIZE, L'\0');
            if ((result = LimpetReadDeviceId(logicalDeviceNumber, &output[0])) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            output.resize(output.find(L'\0'));

            wprintf(L"<DevId>\n  %s\n</DevId>\n", output.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<DevId/>\n");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<DevId/>\n");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_PROV_CONNECTIONSTR)) &&
        (parameters.size() == 1))
    {
        std::wstring& connectionString = parameters[0];

        wprintf(XML_HEADER);
        try
        {
            // This is an example what such a string looks like:
            //HostName=SolarSystem.azure-devices.net;DeviceId=Limpet;SharedAccessKey=b49ia/Vx4JA/s4NHMluV5Is7YEp+EKlfCUHPp4zgau8=

            // Make a lower case copy of the string which we will use to look up the tokens.
            // The base64 encoded data gets garbled up here so we have to make sure to only 
            // read the index locations from it and then transpose that on the original string.
            std::wstring connectionStringLC(connectionString);
            WStringToLower(connectionStringLC);

            // Get the hostname from the string. Define the tokens first.
            const std::wstring hostNameToken(L"hostname=");
            const std::wstring deviceIdToken(L"deviceid=");
            const std::wstring sharedAccessKeyToken(L"sharedaccesskey=");
            const std::wstring separatorToken(L";");

            // Get the hostName
            size_t cursor = connectionStringLC.find(hostNameToken) + hostNameToken.size();
            size_t length = connectionStringLC.find(separatorToken, cursor) - cursor;
            std::wstring hostName = connectionString.substr(cursor, length);
            cursor += length + separatorToken.size();

            // Get the deviceId
            cursor = connectionStringLC.find(deviceIdToken, cursor) + deviceIdToken.size();
            length = connectionStringLC.find(separatorToken, cursor) - cursor;
            std::wstring deviceId = connectionString.substr(cursor, length);
            cursor += length + separatorToken.size();

            // Get the sharedAccessKey
            cursor = connectionStringLC.find(sharedAccessKeyToken, cursor) + sharedAccessKeyToken.size();
            if ((length = connectionStringLC.find(separatorToken, cursor)) == std::wstring::npos)
            {
                // Usually this is the last section and there is no trailing separator
                length = connectionStringLC.size() - cursor;
            }
            else
            {
                // Assuming that there could be technically data following we cut that off here and ignore that
                length -= cursor;
            }
            std::wstring sharedAccessKey = connectionString.substr(cursor, length);
            cursor += length + separatorToken.size();

            // Write the data into the TPM
            std::wstring uri(LIMPET_STRING_SIZE, L'\0');
            uri.clear();
            uri += hostName;
            uri += std::wstring(L"/");
            uri += deviceId;
            std::wstring storageName(LIMPET_STRING_SIZE, L'\0');
            if ((result = LimpetStoreURI(logicalDeviceNumber, (WCHAR*)uri.c_str(), (WCHAR*)storageName.c_str())) != TPM_RC_SUCCESS)
            {
                throw TPM_RC_FAILURE;
            }
            storageName.resize(storageName.find(L'\0'));
            std::wstring hmacKeyName(LIMPET_STRING_SIZE, L'\0');
            CreateHmacKey(logicalDeviceNumber, (WCHAR*)sharedAccessKey.c_str(), (WCHAR*)hmacKeyName.c_str());
            hmacKeyName.resize(hmacKeyName.find(L'\0'));

            // Produce the output
            wprintf(L"<Provisioned>\n  <StorageName>\n    %s\n  </StorageName>\n  <KeyName>\n    %s\n  </KeyName>\n</Provisioned>", storageName.c_str(), hmacKeyName.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<Provisioned/>\n");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<Provisioned/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_STORE_DEVICE_URI)) &&
        (parameters.size() == 1))
    {
        std::wstring& uriString = parameters[0];

        wprintf(XML_HEADER);
        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetStoreURI(logicalDeviceNumber, (WCHAR*)uriString.c_str(), (WCHAR*)output.c_str())) == TPM_RC_SUCCESS)
        {
            output.resize(output.find(L'\0'));
            wprintf(L"<StorageName>\n  %s\n</StorageName>\n", output.c_str());
        }
        else
        {
            wprintf(L"<StorageName/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_READ_DEVICE_URI)) &&
        (parameters.empty()))
    {
        wprintf(XML_HEADER);
        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        try
        {
            ReadURI(logicalDeviceNumber, (WCHAR*)output.c_str());
            output.resize(output.find(L'\0'));
            wprintf(L"<ServiceURI>\n  %s\n</ServiceURI>\n", output.c_str());
        }
        catch (...)
        {
            wprintf(L"<ServiceURI/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_DESTROY_DEVICE_URI)) &&
        (parameters.empty()))
    {
        wprintf(XML_HEADER);
        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetDestroyURI(logicalDeviceNumber, (WCHAR*)output.c_str())) == TPM_RC_SUCCESS)
        {
            output.resize(output.find(L'\0'));
            wprintf(L"<StorageName>\n  %s\n</StorageName>\n", output.c_str());
        }
        else
        {
            wprintf(L"<StorageName/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_CREATE_HMAC_KEY)) &&
        (parameters.size() == 1))
    {
        std::wstring& hmacKeyString = parameters[0];

        wprintf(XML_HEADER);
        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        try
        {
            CreateHmacKey(logicalDeviceNumber, (WCHAR*)hmacKeyString.c_str(), (WCHAR*)output.c_str());
            output.resize(output.find(L'\0'));
            wprintf(L"<KeyName>\n  %s\n</KeyName>\n", output.c_str());
        }
        catch (...)
        {
            wprintf(L"<KeyName/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_SIGN_WITH_HMAC_KEY)) &&
        (parameters.size() == 1))
    {
        std::wstring& dataToSignFileName = parameters[0];

        wprintf(XML_HEADER);
        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        try
        {
            SignWithHmacKey(logicalDeviceNumber, (WCHAR*)dataToSignFileName.c_str(), (WCHAR*)output.c_str());
            output.resize(output.find(L'\0'));
            wprintf(L"<Sha256Hmac>\n  %s\n</Sha256Hmac>\n", output.c_str());
        }
        catch (...)
        {
            wprintf(L"<Sha256Hmac/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_GENERATE_AZURE_TOKEN)) &&
        (parameters.size() < 2))
    {
        std::wstring validityString{ parameters.empty() ? std::wstring() : parameters[0] };
        DWORD validity = 3600;
        FILETIME utcTime = { 0 };
        LARGE_INTEGER expiration = { 0 };

        // Calculate the validity time of the token
        if ((validityString.size() > 0) &&
            (swscanf_s(validityString.c_str(), L"%d", &validity) != 1))
        {
            // We could not really process what was provided so we revert to the default
            validity = 3600;
        }
        GetSystemTimeAsFileTime(&utcTime);
        expiration.HighPart = utcTime.dwHighDateTime;
        expiration.LowPart = utcTime.dwLowDateTime;
        expiration.QuadPart = (expiration.QuadPart / WINDOWS_TICKS_PER_SEC) - EPOCH_DIFFERENCE + validity;

        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        size_t cchOutput = LIMPET_STRING_SIZE;
        if ((result = LimpetGenerateSASToken(logicalDeviceNumber, &expiration, (WCHAR*)output.c_str(), &cchOutput)) == TPM_RC_SUCCESS)
        {
            output.resize(output.find(L'\0'));
            wprintf(L"%s\n", output.c_str());
        }
        else
        {
            wprintf(L"<SASToken/>");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_EVICT_HMAC_KEY)) &&
        (parameters.empty()))
    {
        wprintf(XML_HEADER);
        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        if (LimpetEvictHmacKey(logicalDeviceNumber, (WCHAR*)output.c_str()) == TPM_RC_SUCCESS)
        {
            output.resize(output.find(L'\0'));
            wprintf(L"<KeyName>\n  %s\n</KeyName>\n", output.c_str());
        }
        else
        {
            wprintf(L"<KeyName/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_READ_RSASSA_CERT)) &&
        (parameters.size() < 2))
    {
        std::wstring certFileName{ parameters.empty() ? std::wstring() : parameters[0] };

        try
        {
            // Get the data
            std::vector<BYTE> cert(2048);
            UINT32 cbCert;
            std::vector<BYTE> certThumbPrint(SHA1_DIGEST_SIZE);
            std::vector<BYTE> tpmKey(1024);
            UINT32 cbTpmKey = 0;
            std::vector<BYTE> tpmKeyName(LIMPET_TPM_OBJECT_NAME);
            std::wstring keyName(LIMPET_STRING_SIZE, L'\0');
            if ((result = LimpetReadOrCreateIdentityKeyCert(logicalDeviceNumber,
                BCRYPT_RSA_ALGORITHM,
                &cert[0],
                (UINT32)cert.size(),
                &cbCert,
                &certThumbPrint[0],
                &tpmKey[0],
                (UINT32)tpmKey.size(),
                &cbTpmKey,
                &tpmKeyName[0],
                (WCHAR*)keyName.c_str())) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            cert.resize(cbCert);
            tpmKey.resize(cbTpmKey);
            keyName.resize(keyName.find(L'\0'));

            // Write the cert file if requested
            if (certFileName.size() > 0)
            {
                WriteToDisk(certFileName, cert);
            }

            // Make the the data readable
            std::wstring printableCert = MakePrintable(cert, CRYPT_STRING_BASE64);
            std::wstring printableTpmKey = MakePrintable(tpmKey, CRYPT_STRING_BASE64);
            std::wstring printableTpmKeyName = MakePrintable(tpmKeyName, CRYPT_STRING_BASE64);
            std::wstring printableCertThumbPrint = MakePrintable(certThumbPrint, CRYPT_STRING_HEXRAW);

            // Print the results
            wprintf(XML_HEADER);
            wprintf(L"<RSASSAIDK>\n  <KeyName>\n    %s\n  </KeyName>\n  <IDKCertThumbPrint>\n    %s\n  </IDKCertThumbPrint>\n  <IDKCert>\n    %s\n  </IDKCert>\n  <TPM2B_NAME>\n    %s\n  </TPM2B_NAME>\n  <TPM2B_PUBLIC>\n    %s\n  </TPM2B_PUBLIC>\n</RSASSAIDK>", keyName.c_str(), printableCertThumbPrint.c_str(), printableCert.c_str(), printableTpmKeyName.c_str(), printableTpmKey.c_str());

        }
        catch (UINT32 err)
        {
            wprintf(L"<RSASSAIDK/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<RSASSAIDK/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_READ_ECDSA_CERT)) &&
        (parameters.size() < 2))
    {
        std::wstring certFileName{ parameters.empty() ? std::wstring() : parameters[0] };

        try
        {
            // Get the data
            std::vector<BYTE> cert(2048);
            UINT32 cbCert;
            std::vector<BYTE> certThumbPrint(SHA1_DIGEST_SIZE);
            std::vector<BYTE> tpmKey(1024);
            UINT32 cbTpmKey = 0;
            std::vector<BYTE> tpmKeyName(LIMPET_TPM_OBJECT_NAME);
            std::wstring keyName(LIMPET_STRING_SIZE, L'\0');
            if ((result = LimpetReadOrCreateIdentityKeyCert(logicalDeviceNumber,
                BCRYPT_ECDSA_P256_ALGORITHM,
                &cert[0],
                (UINT32)cert.size(),
                &cbCert,
                &certThumbPrint[0],
                &tpmKey[0],
                (UINT32)tpmKey.size(),
                &cbTpmKey,
                &tpmKeyName[0],
                (WCHAR*)keyName.c_str())) != TPM_RC_SUCCESS)
            {
                throw result;
            }
            cert.resize(cbCert);
            tpmKey.resize(cbTpmKey);
            keyName.resize(keyName.find(L'\0'));

            // Write the cert file if requested
            if (certFileName.size() > 0)
            {
                WriteToDisk(certFileName, cert);
            }

            // Make the the data readable
            std::wstring printableCert = MakePrintable(cert, CRYPT_STRING_BASE64);
            std::wstring printableTpmKey = MakePrintable(tpmKey, CRYPT_STRING_BASE64);
            std::wstring printableTpmKeyName = MakePrintable(tpmKeyName, CRYPT_STRING_BASE64);
            std::wstring printableCertThumbPrint = MakePrintable(certThumbPrint, CRYPT_STRING_HEXRAW);

            // Print the results
            wprintf(XML_HEADER);
            wprintf(L"<ECDSAIDK>\n  <KeyName>\n    %s\n  </KeyName>\n  <IDKCertThumbPrint>\n    %s\n  </IDKCertThumbPrint>\n  <IDKCert>\n    %s\n  </IDKCert>\n  <TPM2B_NAME>\n    %s\n  </TPM2B_NAME>\n  <TPM2B_PUBLIC>\n    %s\n  </TPM2B_PUBLIC>\n</ECDSAIDK>", keyName.c_str(), printableCertThumbPrint.c_str(), printableCert.c_str(), printableTpmKeyName.c_str(), printableTpmKey.c_str());

        }
        catch (UINT32 err)
        {
            wprintf(L"<ECDSAIDK/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<ECDSAIDK/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_DELETE_RSASSA_CERT)) &&
        (parameters.empty()))
    {
        // Drop the key
        wprintf(XML_HEADER);
        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetDestroyIdentityCert(logicalDeviceNumber,
            BCRYPT_RSA_ALGORITHM,
            &output[0])) == TPM_RC_SUCCESS)
        {
            output.resize(output.find(L'\0'));
            wprintf(L"<RSASSAIDK>\n  <KeyName>\n    %s\n  </KeyName>\n</RSASSAIDK>", output.c_str());
        }
        else
        {
            wprintf(L"<RSASSAIDK/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_DELETE_ECDSA_CERT)) &&
        (parameters.empty()))
    {
        // Drop the key
        wprintf(XML_HEADER);
        std::wstring output(LIMPET_STRING_SIZE, L'\0');
        if ((result = LimpetDestroyIdentityCert(logicalDeviceNumber,
            BCRYPT_ECDSA_P256_ALGORITHM,
            &output[0])) == TPM_RC_SUCCESS)
        {
            output.resize(output.find(L'\0'));
            wprintf(L"<RSASSAIDK>\n  <KeyName>\n    %s\n  </KeyName>\n</RSASSAIDK>", output.c_str());
        }
        else
        {
            wprintf(L"<RSASSAIDK/>\n");
            result = TPM_RC_NO_RESULT;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_ACTIVATE_RSASSA_ID)) &&
        (parameters.size() >= 1) && (parameters.size() < 4))
    {
        std::wstring& activationBlobFileName = parameters[0];
        std::wstring payloadFileName((parameters.size() > 1) ? parameters[1] : std::wstring(L""));
        std::wstring payloadKeyFileName((parameters.size() > 2) ? parameters[2] : std::wstring(L""));

        wprintf(XML_HEADER);
        try
        {
            // Read the activation file
            std::vector<BYTE> activationBlob = ReadFromDisk(activationBlobFileName);

            // Get the job done
            std::vector<BYTE> activationSecret(MAX_AES_KEY_BYTES);
            if ((result = LimpetActivateIdentityCert(logicalDeviceNumber,
                BCRYPT_RSA_ALGORITHM,
                &activationBlob[0],
                (UINT32)activationBlob.size(),
                &activationSecret[0])) != TPM_RC_SUCCESS)
            {
                throw result;
            }

            // Decrypt the payload if requested
            if (payloadFileName.size() > 0)
            {
                if ((result = LimpetSymmetricPayloadProtection(false, &activationSecret[0], (WCHAR*)payloadFileName.c_str())) != TPM_RC_SUCCESS)
                {
                    throw result;
                }
            }

            // Write the payload key to a file if requested
            if (payloadKeyFileName.size() > 0)
            {
                WriteToDisk(payloadKeyFileName, activationSecret);
            }

            // Print the results
            std::wstring printableActivationSecret = MakePrintable(activationSecret, CRYPT_STRING_BASE64);
            wprintf(L"<PayloadKey>\n  %s\n</PayloadKey>", printableActivationSecret.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<PayloadKey/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<PayloadKey/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_ACTIVATE_ECDSA_ID)) &&
        (parameters.size() >= 1) && (parameters.size() < 4))
    {
        std::wstring& activationBlobFileName = parameters[0];
        std::wstring payloadFileName((parameters.size() > 1) ? parameters[1] : std::wstring(L""));
        std::wstring payloadKeyFileName((parameters.size() > 2) ? parameters[2] : std::wstring(L""));

        wprintf(XML_HEADER);
        try
        {
            // Read the activation file
            std::vector<BYTE> activationBlob = ReadFromDisk(activationBlobFileName);

            // Get the job done
            std::vector<BYTE> activationSecret(MAX_AES_KEY_BYTES);
            if ((result = LimpetActivateIdentityCert(logicalDeviceNumber,
                BCRYPT_ECDSA_P256_ALGORITHM,
                &activationBlob[0],
                (UINT32)activationBlob.size(),
                &activationSecret[0])) != TPM_RC_SUCCESS)
            {
                throw result;
            }

            // Decrypt the payload if requested
            if (payloadFileName.size() > 0)
            {
                if ((result = LimpetSymmetricPayloadProtection(false, &activationSecret[0], (WCHAR*)payloadFileName.c_str())) != TPM_RC_SUCCESS)
                {
                    throw result;
                }
            }

            // Write the payload key to a file if requested
            if (payloadKeyFileName.size() > 0)
            {
                WriteToDisk(payloadKeyFileName, activationSecret);
            }

            // Print the results
            std::wstring printableActivationSecret = MakePrintable(activationSecret, CRYPT_STRING_BASE64);
            wprintf(L"<PayloadKey>\n  %s\n</PayloadKey>", printableActivationSecret.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<PayloadKey/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<PayloadKey/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_DESROY_LOGICAL_DEV)) &&
        (parameters.empty()))
    {
        wprintf(XML_HEADER);
        try
        {
            std::wstring storageName(LIMPET_STRING_SIZE, L'\0');
            wprintf(L"<Destroyed>\n");
            if ((result = LimpetDestroyURI(logicalDeviceNumber, (WCHAR*)storageName.c_str())) == TPM_RC_SUCCESS)
            {
                storageName.resize(storageName.find(L'\0'));
                wprintf(L"  <StorageName>\n    %s\n  </StorageName>\n", storageName.c_str());
            }
            std::wstring hmacKeyName(LIMPET_STRING_SIZE, L'\0');
            if (LimpetEvictHmacKey(logicalDeviceNumber, (WCHAR*)hmacKeyName.c_str()) == TPM_RC_SUCCESS)
            {
                hmacKeyName.resize(hmacKeyName.find(L'\0'));
                wprintf(L"  <HmacKeyName>\n    %s\n  </HmacKeyName>\n", hmacKeyName.c_str());
            }
            std::wstring rsaKeyName(LIMPET_STRING_SIZE, L'\0');
            if ((result = LimpetDestroyIdentityCert(logicalDeviceNumber,
                BCRYPT_RSA_ALGORITHM,
                (WCHAR*)rsaKeyName.c_str())) == TPM_RC_SUCCESS)
            {
                rsaKeyName.resize(rsaKeyName.find(L'\0'));
                wprintf(L"  <RsaKeyName>\n    %s\n  </RsaKeyName>\n", rsaKeyName.c_str());
            }
            std::wstring ecdsaKeyName(LIMPET_STRING_SIZE, L'\0');
            if ((result = LimpetDestroyIdentityCert(logicalDeviceNumber,
                BCRYPT_ECDSA_P256_ALGORITHM,
                (WCHAR*)ecdsaKeyName.c_str())) == TPM_RC_SUCCESS)
            {
                ecdsaKeyName.resize(ecdsaKeyName.find(L'\0'));
                wprintf(L"  <EcdsaKeyName>\n   %s\n  </EcdsaKeyName>\n", ecdsaKeyName.c_str());
            }
            wprintf(L"</Destroyed>\n");
        }
        catch (UINT32 err)
        {
            wprintf(L"<Destroyed/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<Destroyed/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else if ((command == std::wstring(PARAMETER_IMPORT_HOSTAGE)) &&
        (parameters.size() >= 1) && (parameters.size() < 3))
    {
        std::wstring& hostageFileName = parameters[0];
        std::wstring payloadKeyFileName((parameters.size() > 1) ? parameters[1] : std::wstring(L""));

        wprintf(XML_HEADER);
        try
        {
            // Read the activation file
            std::vector<BYTE> hostage = ReadFromDisk(hostageFileName);

            // Get the job done
            std::vector<BYTE> activationSecret(MAX_AES_KEY_BYTES);
            std::wstring keyName(LIMPET_STRING_SIZE, L'\0');
            std::wstring uriName(LIMPET_STRING_SIZE, L'\0');
            if ((result = LimpetImportHostageKey(logicalDeviceNumber,
                &hostage[0],
                (UINT32)hostage.size(),
                &activationSecret[0],
                (WCHAR*)keyName.c_str(),
                (WCHAR*)uriName.c_str())) != TPM_RC_SUCCESS)
            {
                throw result;
            }

            // Write the payload key to a file if requested
            if (payloadKeyFileName.size() > 0)
            {
                WriteToDisk(payloadKeyFileName, activationSecret);
            }

            // Print the results
            std::wstring printableActivationSecret = MakePrintable(activationSecret, CRYPT_STRING_BASE64);
            wprintf(L"<HostageImport>\n  <PayloadKey>\n    %s\n  </PayloadKey>\n  <KeyName>\n    %s\n  </KeyName>\n  <StorageName>\n    %s\n  </StorageName>\n</HostageImport>", printableActivationSecret.c_str(), keyName.c_str(), uriName.c_str());
        }
        catch (UINT32 err)
        {
            wprintf(L"<HostageImport/>");
            result = err;
        }
        catch (...)
        {
            wprintf(L"<HostageImport/>");
            result = TPM_RC_FAILURE;
        }

        return result;
    }
    else
    {
        GetHelp();
    }

    return (UINT32)result;
}
