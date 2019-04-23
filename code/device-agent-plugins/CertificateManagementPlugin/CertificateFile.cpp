// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CertificateFile.h"
#include <iomanip>
#include <Wincrypt.h>


using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace CertificateManagementPlugin {

    CertificateFile::CertificateFile(const std::string& certFileName)
    {
        std::vector<char> certEncoded;
        LoadFile(certFileName, certEncoded);

        PCCERT_CONTEXT certContext = CertCreateCertificateContext(X509_ASN_ENCODING, reinterpret_cast<BYTE*>(certEncoded.data()), static_cast<DWORD>(certEncoded.size()));
        if (!certContext)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: CertCreateCertificateContext() failed.");
        }

        DWORD thumbPrintSize = 512;
        std::vector<BYTE> thumbPrint(thumbPrintSize);

        BOOL result = CryptHashCertificate(
            NULL, // Not used.
            0,    // algorithm id. Default is 0 (SHA1).
            0,    // flags to be passed to the hash API.
            certContext->pbCertEncoded,
            certContext->cbCertEncoded,
            thumbPrint.data(),
            &thumbPrintSize);

        if (result)
        {
            std::basic_ostringstream<wchar_t> thumbPrintString;
            for (unsigned int i = 0; i < thumbPrintSize; ++i)
            {
                thumbPrintString << std::setw(2) << std::setfill(L'0') << std::hex << thumbPrint[i];
            }
            _thumbPrint = WideToMultibyte(thumbPrintString.str().c_str());
        }

        CertFreeCertificateContext(certContext);

        if (!result)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: CryptHashCertificate() failed.");
        }

        _fullFileName = certFileName;
    }

    std::string CertificateFile::FullFileName() const
    {
        return _fullFileName;
    }
    std::string CertificateFile::ThumbPrint() const
    {
        return _thumbPrint;
    }
}}}}
