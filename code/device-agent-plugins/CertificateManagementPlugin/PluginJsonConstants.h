// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define CertificateManagementHandlerId "certificates"
#define CertificateDetailedInfoHandlerId "GetDetailedCertificateInfoCmd"

#define InstallCertificate "installCertificate"
#define UninstallCertificate "uninstallCertificate"
#define InstalledCertificateInfo "installedCertificateInfo"
#define DownloadCertificate "downloadCertificate"

#define JsonCertificateState "state"
#define JsonCertificateFileName "fileName"
#define JsonCertificateIssuedBy "issuedBy"
#define JsonCertificateIssuedTo "issuedTo"
#define JsonCertificateValidFrom "validFrom"
#define JsonCertificateValidTo "validTo"
#define JsonCertificateBase64Encoding "base64Encoding"
#define JsonCertificateTemplateName "templateName"

#define JsonStateInstalled "installed"
#define JsonStateUninstalled "uninstalled"
#define JsonConnectionString "connectionString"
#define JsonCACertificateInfo "rootCATrustedCertificates_CA"
#define JsonRootCertificateInfo "rootCATrustedCertificates_Root"
#define JsonMyUserCertificateInfo "certificateStore_My_User"
#define JsonMySystemCertificateInfo "certificateStore_My_System"
#define JsonRootSystemCertificateInfo "certificateStore_Root_System"
#define JsonCASystemCertificateInfo "certificateStore_CA_System"
#define JsonTrustedPublisherCertificateInfo "rootCATrustedCertificates_TrustedPublisher"
#define JsonTrustedPeopleCertificateInfo "rootCATrustedCertificates_TrustedPeople"