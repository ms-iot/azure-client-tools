# Universal Windows Platform (UWP) Application Management

## Overview

The **UWP Application Management** functionality allows the operator to perform the following tasks:

- Install or upgrade an application (from a blob storage).
- Uninstall an application.
- List installed applications.
- Managing application lifecycle

## Installing or upgrading an application

Device Management client supports declarative app management with Azure IoT Hub device twin. To install a new application on the device, the opeator uploads the application package file to an Azure Storage container first and then configures the device twin with the file location. 

### Example JSON for installing a new application 

<pre>
"desired" : {
  "uwpApps" : {
    "connectionString" : "Azure storage connection string",
    "apps" : {
      "8112d70e-a549-4378-96a9-63e8491e3d66_7ywy5sjsre78e" : {
      "pkgFamilyName" : "8112d70e-a549-4378-96a9-63e8491e3d66_7ywy5sjsre78e",
      "version" : "1.0.0.0",
      "appxSource" : "dmtest\\UwpApp1_1.0.0.0_x64.appx",
      "depsSources": "dmtest\\Microsoft.NET.Native.Framework.1.7.appx;dmtest\\Microsoft.NET.Native.Runtime.1.7.appx;dmtest\\Microsoft.VCLibs.x64.14.00.appx",
      "startup" : "foreground",
      "launchAfterInstall: : true
     }
    }
  }
}
</pre>

### Details

The `"desired.uwpApps.app"` property contains list of application an operator wants to install on the device. 

Each app is identified by its `"packageFamilyId"` - which is its package family name in a form that can be used as a json property name (for example, you can replace illegal characters with underscores).

- `"pkgFamilyName"`: the package family name of the target application.
- `"version"`: this property indicate the desired state of that application on the device. It can take one of several forms:
    - `Major.Minor.Build.Revision`: This indicates that the specified version is to be installed and reported.
        - If the desired version is the same as the one installed, no installs/uninstalls will take place. However, other applicable settings (like startUp) will be applied.
        - If the desired version is older than the one installed, the installed application will be uninstalled and the specified one will be installed. Applicable settings will be processed afterwards.
        - If the desired version is new than the one install, the specified one will installed on top of the existing one. Applicable settings will be processed.
        - <b>Note</b>: Installing a new application from the store is not supported.
        - `"not installed"` : This indicates that the specified application should not be installed on the device. If it is already installed, the DM client will uninstall it.
- `"startUp"`: the start-up designation of this application. Possible values are:
    - `"none"`: this application is neither the foreground application or a background task.
    - `"foreground"`: this application is the foreground application. Only one application can have `"startUp"` set to this value. When switching from foreground application to another, both application must appear      in the same transaction. The order is not relevant though as the DM client re-orders them.
    - `"background"`: this application is a background task. Any number of applications can be set as background tasks.
- `"appxSource": "container\appPackage.appx"`
    - The appx package file name in the Azure blob storage. The access to the blob storage is performed as described [here](external-storage.md). The value of this property cannot be empty or null if the app is to be installed on the device. Note that this must be an appx package - and not a bundle. In the upgrade scenario, the new appx version must be greater than the one already present. For more details on the requirements, see the [PackageManager](https://docs.microsoft.com/en-us/uwp/api/Windows.Management.Deployment.PackageManager) class.
- `"depsSources": "container\dep0.appx;container\dep1.appx"`
    - The semi-colon separated relative paths (in Azure Storage) of dependencies.
- `"launchAfterInstall": true`
    - `true`: The application will be launched after the installation is complete

## Uninstalling an application

An installed application can be uninstalled by specifying `"not installed"` in the version field. 

### Example JSON for uninstalling an application 

<pre>
"desired" : {
  "uwpApps" : {
    "connectionString" : "Azure storage connection string",
    "apps" : {
      "8112d70e-a549-4378-96a9-63e8491e3d66_7ywy5sjsre78e" : {
        "pkgFamilyName" : "8112d70e-a549-4378-96a9-63e8491e3d66_7ywy5sjsre78e",
        "version" : "not installed",
      }
    }
  }
}
</pre>

<b>Note</b> If an application is marked as `"startup"` as `"foreground"`, it cannot be uninstalled without setting a different application with `"startup"` as `"foreground"`

## Listing installed applications 

To get the list of all the installed applications on a device, operator needs to do a direct method call.

### Method Name 
`GetInstalledUwpAppsCmd`

This method returns all the installed application on the system in following format 

<pre>
"installedApps" :
  [
    {
      "pkgFamilyName" : "App1",
      "version" : "App1 version"
    },
    {
      "pkgFamilyName" : "App2",
       "version" : "App2 version"
    },
    ...
  ]
</pre>

## Managing application lifecycle

For managing application lifecyle, operator needs to do a direct method call.

### Method Name
`ManageAppLifeCycleCmd`

### Parameters 

#### Start an application

<pre>
{
  "pkgFamilyName" : "8112d70e-a549-4378-96a9-63e8491e3d66_7ywy5sjsre78e",
  "action" : "start"
}
</pre>

#### Stop an application 

<pre>
{
  "pkgFamilyName" : "8112d70e-a549-4378-96a9-63e8491e3d66_7ywy5sjsre78e",
  "action" : "stop"
}
</pre>

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md) | [Certificate Management](certificate-management.md) | [UWP Application Management](uwpapplication-management.md)
