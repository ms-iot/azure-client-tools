# Device Agent Configuration File

## Purpose

The Device Agent Configuration File declares a number of static configurations that drive the behavior ot he Device Agent. The configurations are read only once at the start-up of the agent.

## Sample

<pre>
{
  "textLogFilesPath": "..\\logs",
  "pluginManifestsPath": "..\\manifests",
  "pluginsDataPath": "..\\data",
  "sasExpiryInterval": 86400,
  "sasRenewalGracePeriod": 3600,
  "dpsEnabled": true,
  "dpsUri": "",
  "dpsScopeId": "",
  "connectionStringExpiry": 3600,
  "deviceSlotNumber": 0,
  "connectionString": "",
  "dmModuleSlotNumber": 0,
  "dmModuleId": "dmModule",
  "azureInterfaceType": "raw",
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
    },
    "StartFactoryResetCmd": {
      "handlerTestMode": true
    },
    "StartRebootCmd": {
      "handlerTestMode": false
    },
    "StartRemoteWipeCmd": {
      "handlerTestMode": false
    }
  }
}
</pre>

## Fields

### Debugging

| Parameter | Type | Required | Production | Description |
|-----------|------|----------|------------|--------------|
| textLogFilesPath | string | No | No | Set this value to the relative or absolute path of where you want the text log files to be generated. |
| connectionString | string | No | No | Set this value to a connection string for easier testing during development. It bypasses the DPS client initialization. |

### Diagnostic logs

| Parameter | Type | Required | Production | Description |
|-----------|------|----------|------------|--------------|
| pluginsDataPath | string | No | No | Set this value to the relative or absolute path of where you want the diagnostic logs to be generated or logs to be enumerated from. |

### DPS

| Parameter | Type | Required | Production | Description |
|-----------|------|----------|------------|--------------|
| dpsUri | string | Yes | Yes | The service end point: `global.azure-devices-provisioning.net`.|
| dpsScopeId | string | Yes | Yes | The scope Id of your DPS.<br/><br/>You can retrieve this from the 'Overview' tab of your Device Provisioning Service.|
| dpsEnabled | bool | Yes | Yes | If true, DPS provisioning will be invoked if no connection string is present in the TPM. If falls, the device agent will wait for the connection string to be store in the TPM before proceeding. |

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
| handlers | string | No | Yes | This node holds a property group per handler. Each property groups is will be passed to the handler when its `Start()` method is called. |
| &lt;handler_id&gt; | string | No | Yes | This is the id of the handler whose property group is to follow. |
| reportOnConnect | boolean | No | Yes | Some handlers need to be configured so that they report the current device state even without being first configured from the cloud. A good example of this is the DeviceInfo handler. <br/><br/>In such cases, the device builder can tell the DeviceInfo handler to report its status on connection by toggling this flag to true. <br/><br/> To make use of this property, the handler must implement the code to read it and act on its value. |
| handlerTestMode | boolean | No | Yes | Some handlers can define two modes for operations: test mode and production mode. This can be useful in cases where the author of the handler wants an easy way to test most of the functionality of the handler, and limit testing the device state destructive parts to the final stage of production. <br/><br/>Typically, the developer/tester of the handler will configure the handler to run in test mode until they are ready to test it in production mode. <br/></br>To make use of this property, the handler must implement the code to read it and act on its value. |

## Location and Naming

The configuration file must be placed at the same folder as  `AzureDeviceManagementClient.exe`, and must be named `AzureDeviceManagementClient.json`.

----

[Reference](../reference.md)
