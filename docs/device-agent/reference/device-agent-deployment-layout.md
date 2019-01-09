# Device Agent Deployment Layout

## Purpose

This page describes a possible layout for the Device Agent executable and its supporting files on disk. The manifest files (described below) allow other configurations as well.

## Sample

<pre>
c:\deploy\
          bin\
              AzureDeviceManagementClient.exe
              AzureDeviceManagementClient.json
              &lt;plug-in-executable-name&gt;.dll
          manifests\
              &lt;plug-in-executable-name-manifest&gt;.json
          logs\
              &lt;debug-mode-log-files&gt;.log
</pre>

### Dependency Traversal

- `AzureDeviceManagementClient.exe` starts, and looks for `AzureDeviceManagementClient.json` in the same folder. This part is not configurable.
- `AzureDeviceManagementClient.json` tells `AzureDeviceManagementClient.exe`:
    - Where the plug-in manifests are (relative or absolute path).
    - Where debug-mode log files should be stored.
- `AzureDeviceManagementClient.exe` enumerates and loads all files under `manifests`.
    - `&lt;plug-in-executable-name-manifest&gt;.json` tells `AzureDeviceManagementClient.exe` where the plug-in executables are.
    - `AzureDeviceManagementClient.exe` loads the plug-in executables.

----

[Reference](../reference.md)
