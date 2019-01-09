// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

HRESULT InitializeDpsClient();

HRESULT DeinitializeDpsClient();

HRESULT GetEndorsementKeyPub(
    _Out_writes_bytes_all_(*pcbekPub) PBYTE ekPub,
    _Inout_ UINT32 *pcbekPub
);

HRESULT GetRegistrationId(
    __out_ecount_z(*pcchregistrationId) PWSTR registrationId,
    _Inout_ size_t *pcchRegistrationId
);

HRESULT IsDeviceProvisionedInAzure(
    UINT32 tpmSlotNumber,
    _Out_ bool *provisioned
);

HRESULT EmptyTpmSlot(UINT32 tpmSlotNumber);

HRESULT AzureDpsRegisterDeviceWithoutModules(
    UINT32 tpmSlotNumber,
    const std::wstring wdpsUri,
    const std::wstring wdpsScopeId
);

HRESULT AzureDpsRegisterDevice(
    UINT32 tpmSlotNumber,
    const std::string dpsUri,
    const std::string dpsScopeId,
    UINT32 tpmDmModuleSlotNumber,
    const std::string dmModuleId,
    const std::map<std::string, UINT32> otherModules
);
