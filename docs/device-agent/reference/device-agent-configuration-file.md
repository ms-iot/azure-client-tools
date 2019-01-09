# Device Agent Configuration File

## Purpose

The Device Agent Configuration File declares a number of static configurations that drive the behavior ot he Device Agent. The configurations are read only once at the start-up of the agent.

## Sample

<pre>
{
  "textLogFilesPath": "..\\logs",
  "pluginManifestsPath": "..\\manifests",
  "sasExpiryInterval": 86400,
  "sasRenewalGracePeriod": 3600,
  "dpsUri": "",
  "dpsScopeId": "",
  "connectionStringExpiry": 3600,
  "deviceSlotNumber": 0,
  "debugConnectionString": "",
  "dmModuleSlotNumber": 0,
  "dmModuleId": "dmModule",
  "otherModuleIds": {
    "[MODULE ID]": [SLOT NUMBER],
    "[MODULE ID]": [SLOT NUMBER],
  },
  "handlers": {
    "deviceInfo": {
      "reportOnConnect": true
    },
    "__deviceSchemas": {
      "reportOnConnect": true
    }
  }
}
</pre>

## Fields

### Debugging

| Parameter | Type | Required | Production | Description |
|-----------|------|----------|------------|--------------|
| textLogFilesPath | string | No | No | Set this value to the relative or absolute path of where you want the text log files to be generated. |
| debugConnectionString | string | No | No | Set this value to a connection string for easier evelopment testing during development. It bypasses the DPS client initialization. |

### DPS

| Parameter | Type | Required | Production | Description |
|-----------|------|----------|------------|--------------|
| dpsUri | string | Yes | Yes | The service end point: `global.azure-devices-provisioning.net`.|
| dpsScopeId | string | Yes | Yes | The scope Id of your DPS.<br/><br/>You can retrieve this from the 'Overview' tab of your Device Provisioning Service.|

### Connectivity

| Parameter | Type | Required | Production | Description |
|-----------|------|----------|------------|--------------|
| deviceSlotNumber | integer | Yes | Yes | Set this value to the desired TPM slot to be used for storing the device connection string. |
| sasExpiryInterval | integer | Yes | Yes | The number of seconds after which the SAS token retrieved from the TPM will expire and cannot be used to establish or maintain a connection. |
| sasRenewalGracePeriod | integer | Yes | Yes | The number of seconds before `sasExpiryInterval` that the device agent will start attempting to renew the token. |

### Device Twin Modules

| Parameter | Type | Required | Production | Description |
|-----------|------|----------|------------|--------------|
| dmModuleSlotNumber | integer | No | Yes | Set this value to the desired TPM slot to be used for storing the Device Management connection information.<br/><br/>If Device Management should use the device connection information, `dmModuleSlotNumber` and `deviceSlotNumber` should be the same value.<br/><br/>Default Value: 1 |
| dmModuleId | string | No | Yes | Set this value to the desired module identifier for Device Management to use if a module is being used for Device Management.<br/><br/>Default Value: "WindowsIoTDM" |
| otherModuleIds | object | No | Yes | Use this object to configure device twin modules used by your device outside of Device Management.  By adding "[MODULE ID]": [SLOT NUMBER], the agent will create the desired modules and set their connection information in the designated TPM slot.<br/><br/>Default Value: None|

### Handler Configuration

| Parameter | Type | Required | Production | Description |
|-----------|------|----------|------------|--------------|
| handlers | string | No | Yes | . |
| &lt;handler_id&gt; | string | No | Yes | . |
| reportOnConnect | string | No | Yes | . |

## Location and Naming

The configuration file must be placed at the same folder as  `AzureDeviceManagementClient.exe`, and must be named `AzureDeviceManagementClient.json`.

----

[Reference](../reference.md)
