# Plug-in Creation Walk-Through

## Overview

- [Create the Skeleton](#Create-the-Skeleton)
- [Update The Deployment Script](#Update-The-Deployment-Script)
- [Add Plug-in Specific Configuration](#Add-Plug-in-Specific-Configuration)
- [Debugging Plug-in Load Issues](#Debugging-Plugin-Load-Issues)

## Create the Skeleton

### Creating the Skeleton Using The Plugin Creator Tool

The Plugin Creator Tool can be used to generate an entire indirect plug-in skeleton ready to build and deploy.

The tool can be found under the folder AzureDeviceManagementPlugins in the solution. 

The syntax for using it is:
<pre>
PluginCreator.exe <skeletonPluginFolder> <pluginName> <targetFolderParent>
</pre>

For example, to create a new plug-in for a component named SampleComponentName, we can run the following command:

<pre>
cd &lt;repo root&gt;
code\output\x64\Debug\PluginCreator.exe code\AzureDeviceManagementPlugins\SkeletonPlugin SampleComponentName code\AzureDeviceManagementPlugins
</pre>

**Note**: Make sure you review all the generated files and modify them as necessary.
**Note**: If you do not see the newly created plugin as a part of your solution, you might have to add it manually (..add existing project). 

#### Setting up the project

- Add dependencies on the following projects (or their output if using your own solution):
    - `AzureDeviceManagementPluginCommon`
    - `AzureDeviceManagementCommon`
    - `Utilities`.

### Creating the Skeleton Manually

#### Create a New Project


- Start the Create New Project wizard under the 'AzureDeviceManagementPlugins' solution folder.
- Visual C++ | Windows Desktop | Dynamic-Link Library (DLL)
- Name it 'ComponentNamePlugin'.


#### Create The Skeleton Files

(*Replace ComponentName with your actual component name in all files below*)

- [stdafx.h](../skeleton-plugin/stdafx.h)
- [PluginJsonConstants.h](../skeleton-plugin/PluginJsonConstants.h)
- [ComponentNameHandler.h](../skeleton-plugin/ComponentNameHandler.h)
- [ComponentNameHandler.cpp](../skeleton-plugin/ComponentNameHandler.cpp)
- [ComponentNamePlugin.cpp](../skeleton-plugin/ComponentNamePlugin.cpp)
- [ComponentNamePlugin.def](../skeleton-plugin/ComponentNamePlugin.def)


#### Update The Build Configurations


- Open the .vcxproj file of the new plug-in.
- Add the arm configuration (if needed) and enable ARM desktop build by adding the `WindowsSDKDesktopARMSupport` to the `Globals` property group:
    <pre>
    &lt;PropertyGroup Label="Globals"&gt;
        &lt;WindowsSDKDesktopARMSupport&gt;true&lt;/WindowsSDKDesktopARMSupport&gt;
    &lt;/PropertyGroup&gt;
    </pre>
- Add dependencies on the following projects (or their output if using your own solution):
    - `AzureDeviceManagementPluginCommon`
    - `AzureDeviceManagementCommon`
    - `Utilities`.
- Compare the new vcxproj to the checked-in DeviceInfo one - and make sure all settings are present.


#### Create The Plug-in Manifest File


- Create the plug-in manifest file:
    - A sample can be found here: [ComponentNameManifest.json](../skeleton-plugin/ComponentNameManifest.json)
    - Update the name of the `codeFileName` property with the path of where you will place the new plugin dll (relative to where the agent executable will be running from).

References
- [Plug-in Manifest File](../reference/plugin-manifest-file.md).

## Update The Deployment Script

- Open [deploy.create-device-folder.cmd](../../../scripts/deploy.create-device-folder.cmd)
- Copy the new dll to the target location:
    <pre>
    copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\ComponentNamePlugin.dll %EXECUTABLES_FOLDER%
    </pre>
- Copy the new manifest file to the plugin manifests folder:
    <pre>
    copy ..\code\tests\TestCases\Manifests\ComponentNameManifest.json %PLUGIN_MANIFESTS_FOLDER%
    </pre>
- Note that the plugins manifest folder is specified in the `AzureDeviceManagementClient.json` that is placed next to the agent executable.

## Add Handler Specific Configuration

- Open the agent configuration file.
    - A sample can be found here: [AzureDeviceManagementClient.json](../../../code/tests/TestCases/All/AzureDeviceManagementClient.json).
- Add a new `<handler_id>` node under `handlers`.
    - The value of that node will be passed as is to the `Start()` method every time it is called.
    - `<handler_id>` is the string you pass to the `BaseHandler` constructor as the handler's id.
    <pre>
    {
      "handlers": {
        "componentName": {
          "reportOnConnect": true
        }
      }
    }
    </pre>

References
- [Device Agent Configuration File](../reference/device-agent-configuration-file.md).

## Debugging Plugin Load Issues

### Option 1:

Check the verbose logging of the enumeration code. You can enable verbose logging by following the steps described in the [Debugging](debugging.md) page.

### Option 2:

- Set a break point at `AzureDMClient::EnumeratePlugins`.
- Start the `AzureDeviceManagementClient` under the debugger (enumeration takes place at the very beginning and there will be no time to attach).

----

## Related Topics

- [Extensibility and Discovery Model](extensibility-and-discovery-model.md).
- [Handler Enumeration and Initialization Flow](extensibility-and-discovery-model/handler-enumeration-flow.md).
- [Debugging](debugging.md).

----

[Development Scenario](../development-scenario.md)
