// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "stdafx.h"
#include <string>
#include "..\Utilities\ResultMacros.h"
#include "..\Utilities\TpmSupport.h"
#include "DpsDeviceRegistration.h"

#define TPM20_MAX_SLOT_NUMBER 9

using namespace Microsoft::Azure::DeviceManagement::Utils;

using namespace std;
using namespace std::chrono;
using namespace Microsoft::WRL;


HRESULT  InitializeDpsClient()
{
    return TpmSupport::InitializeTpm();
}

HRESULT  DeinitializeDpsClient()
{
    return TpmSupport::DeinitializeTpm();
}

HRESULT  GetEndorsementKeyPub(
    _Out_writes_bytes_all_(*pcbekPub) PBYTE ekPub,
    _Inout_ UINT32 *pcbekPub
)
{
    HRESULT hr = S_OK;
    RETURN_HR_IF_NULL(E_INVALIDARG, ekPub);
    RETURN_HR_IF_NULL(E_INVALIDARG, pcbekPub);

    TBS_RESULT result = TBS_SUCCESS;
    result = LimpetGetTpmKey2BPub(LIMPET_TPM20_ERK_HANDLE, ekPub, (UINT32)*pcbekPub, pcbekPub, NULL);
    hr = static_cast<HRESULT>(result);

    return hr;
}

HRESULT  GetRegistrationId(
    __out_ecount_z(*pcchregistrationId) PWSTR registrationId,
    _Inout_ size_t *pcchregistrationId
)
{
    HRESULT hr = S_OK;
    RETURN_HR_IF_NULL(E_INVALIDARG, registrationId);
    RETURN_HR_IF_NULL(E_INVALIDARG, pcchregistrationId);

    std::wstring localregistrationId;
    hr = TpmSupport::GetRegistrationId(localregistrationId);

    if (SUCCEEDED(hr) &&
        (*pcchregistrationId > localregistrationId.length() + 1) &&
        (wcscpy_s(registrationId, *pcchregistrationId, localregistrationId.c_str()) == 0))
    {
        *pcchregistrationId = localregistrationId.length();
    }
    else
    {
        *pcchregistrationId = 0;
    }
    return hr;
}

HRESULT  IsDeviceProvisionedInAzure(UINT32 tpmSlotNumber, _Out_ bool *provisioned)
{
    return TpmSupport::IsDeviceProvisioned(tpmSlotNumber, provisioned);
}

HRESULT  EmptyTpmSlot(UINT32 tpmSlotNumber)
{
    return TpmSupport::EmptyTpmSlot(tpmSlotNumber);
}

HRESULT AzureDpsRegisterDeviceWithoutModules(
    UINT32 tpmSlotNumber,
    const std::wstring wdpsUri,
    const std::wstring wdpsScopeId)
{
    RETURN_HR_IF(E_INVALIDARG, tpmSlotNumber > TPM20_MAX_SLOT_NUMBER);
    RETURN_HR_IF(E_INVALIDARG, wdpsUri.empty());
    RETURN_HR_IF(E_INVALIDARG, wdpsScopeId.empty());

    return DpsRegisterDeviceWithoutModules(tpmSlotNumber, wdpsUri, wdpsScopeId);
}

HRESULT AzureDpsRegisterDevice(
    UINT32 tpmSlotNumber,
    const std::string dpsUri,
    const std::string dpsScopeId,
    UINT32 tpmDmModuleSlotNumber,
    const std::string dmModuleId,
    const std::map<std::string, UINT32> otherModules)
{
    RETURN_HR_IF(E_INVALIDARG, tpmSlotNumber > TPM20_MAX_SLOT_NUMBER);
    RETURN_HR_IF(E_INVALIDARG, dpsUri.empty());
    RETURN_HR_IF(E_INVALIDARG, dpsScopeId.empty());
    RETURN_HR_IF(E_INVALIDARG, tpmDmModuleSlotNumber > TPM20_MAX_SLOT_NUMBER);
    RETURN_HR_IF(E_INVALIDARG, tpmSlotNumber != tpmDmModuleSlotNumber && dmModuleId.empty());

    for (auto& mod : otherModules) {
        RETURN_HR_IF(E_INVALIDARG, mod.first.empty());
        RETURN_HR_IF(E_INVALIDARG, mod.second > TPM20_MAX_SLOT_NUMBER);
        RETURN_HR_IF(E_INVALIDARG, mod.second == tpmSlotNumber);
        RETURN_HR_IF(E_INVALIDARG, mod.second == tpmDmModuleSlotNumber);
    }

    return DpsRegisterDeviceAndModules(tpmSlotNumber, dpsUri, dpsScopeId, tpmDmModuleSlotNumber, dmModuleId, otherModules);
}
