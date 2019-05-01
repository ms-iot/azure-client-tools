// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

typedef std::vector<BYTE> keybuffer_t;

class TpmSupport
{
public:

    static HRESULT ImportKeyToTpm(UINT32 logicalDeviceNumber, unsigned char* pbKeyBlob, size_t cbKeyBlob);

    static HRESULT GetEndorsementKeyPub(std::vector<unsigned char>&);

    static HRESULT GetStorageRootKeyPub(std::vector<unsigned char>&);

    static HRESULT InitializeTpm();

    static HRESULT DeinitializeTpm();

    static HRESULT IsDeviceProvisioned(UINT32 logicalDeviceNumber, _Out_ bool *provisioned);

    static HRESULT EmptyTpmSlot(UINT32 logicalDeviceNumber);

    static HRESULT LimpetGenerateDpsSASToken(
        const std::wstring &authenticationKey, // hostage blob sent by DPS service
        std::wstring &tokenScope,
        __int64 expiration,
        _Out_ std::wstring& szConnectionString
    );

    static HRESULT StoreIothubUri(__in UINT32 tpmSlotNumber, __in const std::wstring &uri, __out std::wstring &name);
    static HRESULT GetIothubUri(__in UINT32 tpmSlotNumber, std::string &uriData);
    static HRESULT StoreHmacKey(__in UINT32 tpmSlotNumber, const std::string &key);
    static HRESULT GetRegistrationId(std::wstring &registrationId);

    static HRESULT GetAzureConnectionString(_In_ UINT32 logicalDeviceNumber, _In_ DWORD expiryDurationInSeconds, _Out_ std::wstring& connectionString);

    static HRESULT ClearTpm();
};

}}}};
