# Enabling Plug-ins

### Overview

The service looks for the plug-in manifests folder in its json configuration file. 

- It will then enumerate all manifests found in that folder and load each.
- For each manifest, it will look at where the plug-in dll is placed, load it from there, and register it as a handler.

### Ready-to-Use Plugins

The solution comes with a few plug-ins under the `code\AzureDeviceManagementPlugins` folder and they get placed next to the service exe under `code\output\<arch>\<flavor>`.

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