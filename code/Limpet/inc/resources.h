// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define LIMPET_VERSION (2)

#define GLOBAL_COMMAND_HELP_PUBLIC \
L"Microsoft Windows 10 Core IoT 2016\nThis is Limpet.exe, the persistent deviceID provisioning tool for TPM20.\n" \
L"\n" \
L"Global Commands in the form:\n" \
L"Limpet.exe [Command]\n" \
L" -VER => Show Limpet.exe version\n" \
L" -LAT => List supported add-on TPMs\n" \
L" -IAT [IDx] => Install specified add-on TPM\n" \

#define TPMAVAILABLE_COMMAND_HELP_PUBLIC \
L"\nGlobal TPM Present Commands:\n" \
L" -GTI => Get TPM Info\n" \
L" -FCT => Force Clear TPM\n" \
L" -PFX [PFXFile] [password] => Import PFX file into TPMKSP and machine \"My\" Store\n" \
L" -LLD => List logical devices in use\n" \
L" -ERK {File} => Get TPM Endorsement RSA Key and optionally store\n" \
L"\n" \
L"Logical Device Commands in the form:\n" \
L"Limpet.exe [LogicalDeviceNo 0..9] [Command] [Parameter] {Optional}\n" \
L" -RID => Read Device Id\n" \
L" -SUR [URI] => Store Service URI\n" \
L" -RUR => Read Service URI\n" \
L" -DUR => Destroy Service URI\n" \
L"\n" \
L"HMAC Symmetric Identities:\n" \
L" -CHK [HmacKey] => Create persisted HMAC Key\n" \
L" -AST {Validity} => Generate the SAS token-based connection string (default validity 3600s)\n" \
L" -SHK [File] => Sign data in file with persisted HMAC Key\n" \
L" -EHK => Evict persisted HMAC Key\n" \
L"\n" \
L"Azure device provisioning cloud service(dps) commands in the form:\n" \
L"Limpet.exe [-azuredps [<dps connection string>] [command] [parameters]\r\n" \
L" -azuredps -register <tpm slot number> <global dps uri end point> <scope id> => register the current running device in Iothub using dps service.\n" \
L"     <tpm slot number>: tpm slot number to store the key.\n" \
L"     <global dps service uri>: it is generally fixed global.azure-devices-provisioning.net\n" \
L"     <scope id>: customer dps scope id. you can get this information from azure portal dps service ->overview ->id scope.\n" \
L" -azuredps -enrollmentinfo <csv/json/txt> => read device tpm information needed for enrolling the device later in dps and output in given format.\n" \
L"     default output format is txt.\r\n" \
L"\n" \

#define GLOBAL_COMMAND_HELP_ALL \
L"Microsoft Windows 10 Core IoT 2016\nThis is Limpet.exe, the persistent deviceID provisioning tool for TPM20.\n" \
L"\n" \
L"Global Commands in the form:\n" \
L"Limpet.exe [Command]\n" \
L" -VER => Show Limpet.exe version\n" \
L" -LAT => List supported add-on TPMs\n" \
L" -IAT [IDx] => Install specified add-on TPM\n" \
L" -CAI [ERKFile] [Cert] [ChallengeFile] {PayloadFile} {PayloadKeyFile} => Challenge asym Identity Key\n" \
L" -CSI [ERKFile] [SRKFILE] [hmacKey] [DevId] [URL] [HostageFile] {PayloadKeyFile} => Create bound sym Identity Key\n" \
L" -AIC [CaCert] [Cert] [DevId] [URL] => Authority issue Identity Certificate\n" \
L" -EPL [PayloadKey or File] [PayloadFile] => Encrypt Payload\n" \
L" -DPL [PayloadKey or File] [PayloadFile] => Decrypt Payload\n" \
L" -DCS [Store] => Dump Certificate Store {My, Trust, Ca, Root}\n" \
L" -ICS [Cert] [Store] => Import Certificate to Store {My, Trust, Ca, Root}\n" \

#define TPMAVAILABLE_COMMAND_HELP_ALL \
L"\nGlobal TPM Present Commands:\n" \
L" -GTI => Get TPM Info\n" \
L" -FCT => Force Clear TPM\n" \
L" -PFX [PFXFile] [password] => Import PFX file into TPMKSP and machine \"My\" Store\n" \
L" -LLD => List logical devices in use\n" \
L" -SRK {File} => Get TPM Storage Root Key and optionally store\n" \
L" -ERK {File} => Get TPM Endorsement RSA Key and optionally store\n" \
L" -IIC [Cert] => Import authority issued Identity Certificate\n" \
L" -SIC [Cert] [File] => Sign data file with Identity Cert\n" \
L"\n" \
L"Logical Device Commands in the form:\n" \
L"Limpet.exe [LogicalDeviceNo 0..9] [Command] [Parameter] {Optional}\n" \
L" -RID => Read Device Id\n" \
L" -SUR [URI] => Store Service URI\n" \
L" -RUR => Read Service URI\n" \
L" -DUR => Destroy Service URI\n" \
L" -DLD => Destroy the entire logical device\n" \
L"\n" \
L"HMAC Symmetric Identities:\n" \
L" -PCS [ConnectionString] => Provision static Connection String\n" \
L" -CHK [HmacKey] => Create persisted HMAC Key\n" \
L" -ISI [HostageFile] {PayloadKeyFile} => Import authority issued TPM bound Symetric Identity\n" \
L" -AST {Validity} => Generate dynamic Azure SAS Token (default validity 3600s)\n" \
L" -SHK [File] => Sign data in file with persisted HMAC Key\n" \
L" -EHK => Evict persisted HMAC Key\n" \
L"\n" \
L"ECDSA Asymmetric Identities:\n" \
L" -REC {Cert} => Create or read ECDSA Identity Key Certificate\n" \
L" -AEI [ChallengeFile] {PayloadFile} {PayloadKeyFile} - Activate ECDSA Identity\n" \
L" -DEC => Destroy ECDSA Identity Key and Certificate\n" \
L"\n" \
L"RSASSA Asymmetric Identities:\n" \
L" -RRC {Cert} => Create or read RSASSA Identity Key Certificate\n" \
L" -ARI [ChallengeFile] {PayloadFile} {PayloadKeyFile} - Activate RSASSA Identity\n" \
L" -DRC => Destroy RSASSA Identity Key and Certificate\n" \
L"\n" \
L"Azure device provisioning cloud service(dps) commands in the form:\n" \
L"Limpet.exe [-azuredps [<dps connection string>] [command] [parameters]\r\n" \
L" -azuredps -register <tpm slot number> <global dps uri end point> <scope id> => register the current running device in Iothub using dps service.\n" \
L"     <tpm slot number>: tpm slot number to store the key.\n" \
L"     <global dps service uri>: it is generally fixed global.azure-devices-provisioning.net\n" \
L"     <scope id>: customer dps scope id. you can get this information from azure portal dps service ->overview ->id scope.\n" \
L" -azuredps -enrollmentinfo <csv/json/txt> => read device tpm information needed for enrolling the device later in dps and output in given format.\n" \
L"     default output format is txt.\r\n" \
L"\n" \


#define XML_HEADER L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"

// Command-line parameters
#define PARAMETER_BREAK_ON_ENTER       L"-boe"
#define PARAMETER_FULL_HELP            L"-hhh"
#define PARAMETER_LIMPET_VERSION       L"-ver"
#define PARAMETER_LIST_ADDON_TPMS      L"-lat"
#define PARAMETER_INSTALL_ADDON_TPM    L"-iat"
#define PARAMETER_GET_TPM_INFO         L"-gti"
#define PARAMETER_FORCE_CLEAR_TPM      L"-fct"
#define PARAMETER_PFX_IMPORT           L"-pfx"
#define PARAMETER_LIST_LOGICAL_DEVICES L"-lld"
#define PARAMETER_GET_SRK              L"-srk"
#define PARAMETER_GET_ERK              L"-erk"
#define PARAMETER_READ_DEVICEID        L"-rid"
#define PARAMETER_PROV_CONNECTIONSTR   L"-pcs"
#define PARAMETER_STORE_DEVICE_URI     L"-sur"
#define PARAMETER_READ_DEVICE_URI      L"-rur"
#define PARAMETER_DESTROY_DEVICE_URI   L"-dur"
#define PARAMETER_CREATE_HMAC_KEY      L"-chk"
#define PARAMETER_IMPORT_HOSTAGE       L"-isi"
#define PARAMETER_SIGN_WITH_HMAC_KEY   L"-shk"
#define PARAMETER_GENERATE_AZURE_TOKEN L"-ast"
#define PARAMETER_EVICT_HMAC_KEY       L"-ehk"
#define PARAMETER_READ_ECDSA_CERT      L"-rec"
#define PARAMETER_ACTIVATE_ECDSA_ID    L"-aei"
#define PARAMETER_DELETE_ECDSA_CERT    L"-dec"
#define PARAMETER_READ_RSASSA_CERT     L"-rrc"
#define PARAMETER_ACTIVATE_RSASSA_ID   L"-ari"
#define PARAMETER_DELETE_RSASSA_CERT   L"-drc"
#define PARAMETER_CHALLENGE_ASYM_ID    L"-cai"
#define PARAMETER_CREATE_BOUND_SYM_ID  L"-csi"
#define PARAMETER_ACTIVATE_SYM_ID      L"-asi"
#define PARAMETER_ENCRYPT_PAYLOAD      L"-epl"
#define PARAMETER_DECRYPT_PAYLOAD      L"-dpl"
#define PARAMETER_AUTH_ISSUE_CERT      L"-aic"
#define PARAMETER_SIGN_WITH_CERT       L"-sic"
#define PARAMETER_IMPORT_AUTH_CERT     L"-iic"
#define PARAMETER_DESROY_LOGICAL_DEV   L"-dld"
#define PARAMETER_DUMP_CERTSTORE       L"-dcs"
#define PARAMETER_IMPORT_CERTIFICATE   L"-ics"
#define PARAMETER_AZUREDPS             L"-azuredps"
#define PARAMETER_AZUREIOTHUB          L"-azureiothub"

