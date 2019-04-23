// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "LocalDMClient.h"

#include "LimpetApi/inc/LimpetApi.h"
#include "UrchinLib.h"

#include "../uwp-bridge-lib/DMBridge/DMBridgeServer.h"

using namespace Microsoft::Azure::DeviceManagement::Common;
using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

LocalDMClient::LocalDMClient(
    std::shared_ptr<ServiceParameters> serviceParameters,
    std::shared_ptr<IRawHandlerRouter> rawHandlerRouter) :
    _serviceParameters(serviceParameters),
    _rawHandlerRouter(rawHandlerRouter)
{
}

void LocalDMClient::InvokeReboot()
{
    Json::Value emptyParametersJson;
    InvokeHandlerResult result = _rawHandlerRouter->InvokeHandler("StartRebootCmd", emptyParametersJson);
    // ToDo: communicate the return code?
}

void LocalDMClient::SetScopeId(const std::wstring& scopeId)
{
    auto multibyteScopeId = WideToMultibyte(scopeId.c_str());
    _serviceParameters->SetDpsScopeId(multibyteScopeId);
    _serviceParameters->Save();
}

void LocalDMClient::GetRegistrationId(std::wstring& registrationId)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    HRESULT hr = static_cast<HRESULT>(LimpetInitialize());
    if (FAILED(hr))
    {
        throw DMException(DMSubsystem::Limpet, hr, "LimpetInitialize failed!");
    }

    if ((hr = LimpetGetRegistrationId(registrationId)) != S_OK)
    {
        TRACELINEP(LoggingLevel::Verbose, "Failed to LimpetGetRegistrationId(). Error: ", hr);
        throw DMException(DMSubsystem::Limpet, hr, "LimpetGetRegistrationId failed!");
    }
}

void LocalDMClient::GetEndorsementKey(std::wstring& endorsementKey)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    HRESULT hr = static_cast<HRESULT>(LimpetInitialize());
    if (FAILED(hr))
    {
        throw DMException(DMSubsystem::Limpet, hr, "LimpetInitialize failed!");
    }

    std::vector<BYTE> keyBuffer(1024);
    UINT32 cbKeyBuffer = (UINT32)keyBuffer.size();

    // Get the public key blob
    if ((hr = LimpetGetTpmKey2BPub(LIMPET_TPM20_ERK_HANDLE, &keyBuffer[0], (UINT32)keyBuffer.size(), &cbKeyBuffer, NULL)) != TPM_RC_SUCCESS)
    {
        TRACELINEP(LoggingLevel::Verbose, "Failed to LimpetGetTpmKey2BPub(). Error: ", hr);
        throw DMException(DMSubsystem::Limpet, hr, "LimpetGetTpmKey2BPubfailed!");
    }
    keyBuffer.resize(cbKeyBuffer);

    DWORD cchPrintableData = 0;
    if (CryptBinaryToStringW(&keyBuffer[0], (DWORD)keyBuffer.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &cchPrintableData) == FALSE)
    {
        throw DMException(DMSubsystem::Windows, E_FAIL, "CryptBinaryToStringW failed!");
    }
    endorsementKey.resize(cchPrintableData);
    if (CryptBinaryToStringW(&keyBuffer[0], (DWORD)keyBuffer.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, (LPWSTR)endorsementKey.c_str(), &cchPrintableData) == FALSE)
    {
        throw DMException(DMSubsystem::Windows, E_FAIL, "CryptBinaryToStringW failed!");
    }
}

void LocalDMClient::GetConnetionString(int slot, long long expiry, std::wstring& connectionString)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    HRESULT hr = static_cast<HRESULT>(LimpetInitialize());
    if (FAILED(hr))
    {
        throw DMException(DMSubsystem::Limpet, hr, "LimpetInitialize failed!");
    }

    size_t bufferSize = LIMPET_STRING_SIZE;
    connectionString.resize(bufferSize);

    FILETIME utcTime = { 0 };
    LARGE_INTEGER expiration = { 0 };

    GetSystemTimeAsFileTime(&utcTime);
    expiration.HighPart = utcTime.dwHighDateTime;
    expiration.LowPart = utcTime.dwLowDateTime;
    expiration.QuadPart = (expiration.QuadPart / WINDOWS_TICKS_PER_SEC) - EPOCH_DIFFERENCE + expiry;

    if ((hr = LimpetGenerateSASToken(slot, &expiration, &connectionString[0], &bufferSize)) != S_OK)
    {
        TRACELINEP(LoggingLevel::Verbose, "Failed to LimpetGetRegistrationId(). Error: ", hr);
        throw DMException(DMSubsystem::Limpet, hr, "LimpetGenerateSASToken failed!");
    }
}

}}}}