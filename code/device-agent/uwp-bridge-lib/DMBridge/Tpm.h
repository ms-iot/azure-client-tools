// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

class Tpm
{
public:
    static HRESULT GetEndorsementKey(_Outptr_ int &size, _Outptr_ wchar_t *&endorsementKey);
    static HRESULT GetRegistrationId(_Outptr_ int &size, _Outptr_ wchar_t *&registrationId);
    static HRESULT GetConnectionString(_In_ int slot, _In_ long long expiry, int &size, _Outptr_ wchar_t *&connectionString);
};