# Adding a New Plugin to The Agent Solution

## Overview

In this page, we show how to add a new plug-in to be part of the agent solution - i.e. builds with it, deploys with it for testing, etc.
Those are the most commonly used ones by our customers and are created and maintained by the Microsoft team owning the Device Agent.

If you are considering authoring a new plugin for the agent, it is best to develop it in a separate solution/project.
See instuctions ([here](plugin-build-standalone.md)) on this.

## Create the Skeleton

### Creating the Skeleton Using The Plugin Creator Tool

The Plugin Creator Tool can be used to generate an entire indirect plug-in skeleton ready to build and deploy.

The tool can be found under the folder AzureDeviceManagementPlugins in the solution. 

The syntax for using it is:
<pre>
PluginCreator.exe &lt;skeletonPluginFolder&gt; &lt;pluginName&gt; &lt;targetFolderParent&gt;
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

----

## Related Topics

- [Extensibility and Discovery Model](../extensibility-and-discovery-model.md).
- [Handler Enumeration and Initialization Flow](../extensibility-and-discovery-model/handler-enumeration-flow.md).
- [Debugging](debugging.md).

----

[Home](../../../README.md) | [Device Agent](../device-agent.md) | [Development Scenario](../development-scenario.md) | [Plug-in Creation Walk-Through](developer-plugin-creation.md)

