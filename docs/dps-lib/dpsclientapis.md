# DPS Client Library APIs

The DPS Client APIs are a set of C APIs that allow a device client to provision a device with an Azure IoT Hub identity through the use of the Azure Device Provisioning Service.

#### InitializeDpsClient
This API initializes the DPS client. Internally, this initializes the Urchin objects to work with TPM. Note that this API must be called before using the DPS Client APIs.

<pre>HRESULT InitializeDpsClient()</pre>

#### GetEndorsementKeyPub
This API deinitializes the DPS client. Internally, this deinitializes the Urchin objects that were created as part of InitializeDpsClient(). Note that this API must be called in the clean-up portion when using DPS Client APIs.

<pre>HRESULT DeinitializeDpsClient()</pre>

#### GetEndorsementKeyPub
This API reads the TPM endorsement key public value.

<pre>HRESULT GetEndorsementKeyPub(
    _Out_writes_bytes_all_(*pcbekPub) PBYTE ekPub,
    _Inout_ UINT32 *pcbekPub
)</pre>

| Parameter | Description |
|----|----|
|   ekPub  |  - user has to pass sufficient byte buffer to store public value of endorsement key. On succesful reading, buffer will be updated with endorsement key public value.|
|  pcbekPub| - user has to pass count of passed buffer. On successful reading, it updates with actual endorsement key public value byte count.|
|  returns | S_OK on successful reading and other relavant error code for failure.|

#### GetRegistrationId
This API gets the device registration ID by reading the TPM. This is needed in order to upload the device information to Azure's DPS.

<pre>HRESULT GetRegistrationId(
    __out_ecount_z(*pcchregistrationId) PWSTR registrationId,
    _Inout_ size_t *pcchRegistrationId
)
</pre>


| Parameter | Description |
|----|----|
|    registrationId | user has to pass sufficient WCHAR buffer to store device registration id. On succesful reading, buffer will be updated with device registration id.|
|    pcchRegistrationId | user has to pass size of passed WCHAR buffer. On successful reading, it updates with actual device registration id size.|
|    returns | S_OK on successful reading and other relavant error code for failure.|

#### IsDeviceProvisionedInAzure

This API reads the TPM logical slot and checks whether the Azure connection string is already provisioned or not. Note that this API does not validate by communicating with Azure services.

<pre>HRESULT IsDeviceProvisionedInAzure(
    UINT32 tpmSlotNumber,
    _Out_ bool *provisioned
)
</pre>

| Parameter | Description |
|----|----|
|    tpmSlotNumber | user has to pass logical tpm slot number to check for azure connection string.|
|    provisioned | user has to pass bool pointer to provisioned. It will be updated with true if connection string present in TPM else updated with false.|
|    returns | S_OK on successful reading and other relavant error code for failure.|

#### EmptyTpmSlot
This API erases data stored in the specified TPM logical slot.

<pre>HRESULT EmptyTpmSlot(UINT32 tpmSlotNumber)</pre>

| Parameter | Description |
|----|----|
|    tpmSlotNumber | user has to pass TPM logical slot number to erase data.| 
|    returns | S_OK on successful reading and other relavant error code for failure.|

#### GetAzureConnectionString
This API gets the Azure connection string (SAS token-based) by reading the hostage key and service URL stored in TPM.

<pre>HRESULT GetAzureConnectionString(
    __in UINT32 tpmSlotNumber,
    __in DWORD expiryDurationInSeconds,
    _In_opt_ PCWSTR moduleId,
    __out_ecount_z(*pcchConnectionString) PWSTR connectionString,
    _Inout_ size_t *pcchConnectionString
)
</pre>

| Parameter | Description |
|----|----|
|    tpmSlotNumber | user has pass tpm logical slot number to read hostage key and service url.|
|    expiryDurationInSeconds | user has to pass sas token expiry duration in seconds.|
|    moduleId | Currently this is not implemented.|
|    connectionString | User has to pass sufficient WCHAR buffer to store the azure connection string. On successful reading buffer will be updated with sas token based azure connection string.|
|    pcchConnectionString | User has to pass the WCHAR buffer count of azure connection string. On successful reading, it will be updated with actual buffer count of the returned azure connection string.|
|    returns | S_OK on successful reading and other relavant error code for failure.|

#### AzureDpsRegisterDevice
This API registers the device with IoT Hub by using Azure DPS.

Before using this API, you will need to sign up with Azure and subscribe to IoT Hub and DPS then upload the device information to DPS. Please see this repo's readme.md for more details.

<pre>HRESULT AzureDpsRegisterDevice(
    UINT32 tpmSlotNumber,
    const std::wstring dpsUri,
    const std::wstring dpsScopeId
)
</pre>

| Parameter | Description |
|----|----|
|   tpmSlotNumber | user has to pass TPM slot number to store the key.|
|   dpsUri | user has to pass azure device provisioning service global end point.|
|   dpsScopeId | user has to pass customer dps scope id. You can get this information from azure portal dps service->overview->id scope.|
|   returns | S_OK on successful reading and other relavant error code for failure.|

