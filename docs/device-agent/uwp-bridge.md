# UWP Bridge

## Overview

The UWP Bridge is a WinRT library that allows a UWP application to connect to the Azure device agent and invoke some of its functionality.

After building the projects under the AzureDeviceManagementBridge sub-folder, add the DMBridgeComponent as a reference to your UWP.

Below, you can find the functionality exposed by the DMBridgeComponent.

## DPS Interface

<pre>
interface Dps
{
    HRESULT SetScopeIdRpc([in, string] const wchar_t *scopeId);
}
</pre>

#### Sample

<pre>
using namespace UwpAppUsingDMBridge;
...
auto dps = ref new DpsBridge();
dps->SetScopeId("0x123456");
</pre>

## Reboot Interface

<pre>
interface Reboot
{
    HRESULT InvokeRpc();
}
</pre>

#### Sample

<pre>
    auto reboot = ref new RebootBridge();
    reboot->Invoke();
</pre>

### TPM Interface

<pre>
interface Tpm
{
    HRESULT GetEndorsementKeyRpc([out] int *size, [out, size_is(, *size)] wchar_t **endorsementKey);
    HRESULT GetRegistrationIdRpc([out] int *size, [out, size_is(, *size)] wchar_t **registrationId);
    HRESULT GetConnectionStringRpc([in] INT32 slot, [in] long long expiry, [out] int *size, [out, size_is(, *size)] wchar_t **connectionString);
}
</pre>

#### Sample

<pre>
    using namespace UwpAppUsingDMBridge;
    ...

    auto tpm = ref new TpmBridge();

    // Get registration id from TPM
    auto registrationId = tpm->GetRegistrationId();

    // Get endorsement key from TPM
    auto ek = tpm->GetEndorsementKey();

    // Get connection string from TPM
    auto slotNumberValue = 0;
    auto cxnString = tpm->GetConnectionString(slotNumberValue, 36000);
</pre>
----

[Home](../../README.md) | [Device Agent](device-agent.md)