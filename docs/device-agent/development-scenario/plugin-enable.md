# Enabling Plug-ins

### Overview

The device agent discovers available plug-ins by enumerating the plug-in manifests in the manifests folder.
The manifests folder is specified in the `pluginManifestsPath` attribute in the device agent configuration file ([AzureDeviceManagementClient.json](../reference/device-agent-configuration-file.md)).

Each plug-in manifest tells the device agents where the plug-in binary is, what handlers the plug-in contains/implements, and a number of other attributes.

### Ready-to-Use Plugins

This repo comes with a number of plug-ins under the `code\AzureDeviceManagementPlugins` folder. Each plug-in produces a binary and defines a plug-in manifest.
After building the binaries, the device build needs to:
- Copy the produced binary (from `code\output\<arch>\<flavor>`) to the production layout (for example, next to the AzureDeviceManagementClient.exe).
- Copy the manifest (for example, from `code\AzureDeviceManagementPlugins\DeviceInfoPlugin\DeviceInfoManifest.json`) to the location where the device agent will look for manifests (`pluginManifestsPath`).
    - Make sure that each manifest points to the correct location of the corresponding binary.

### Configuring

To configure the service to enumerate those plug-ins, configure the service with the manifests folder as follows:

- Create a json file `AzureDeviceManagementClient.json`.
- Place it in the same folder as `AzureDeviceManagementclient.exe`.
- Set the value of `pluginManifestsPath` to where the manifests are (sample manifests are stored at `code\tests\TestCases\Manifests`):
    <pre>
    {
      "pluginManifestsPath": "c:\\temp\\dm\\manifests",
    }
    </pre>

- Restart the service.

<pre>
net stop AzureDeviceManagementClient
net start AzureDeviceManagementClient
</pre>

----

### Related Topics

- [Extensibility and Discovery Model](extensibility-and-discovery-model.md).
- [Handler Enumeration and Initialization Flow](extensibility-and-discovery-model/handler-enumeration-flow.md).

----

[Development Scenario](../development-scenario.md)