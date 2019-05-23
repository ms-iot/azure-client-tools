# OEM Device Setup

In a production environment, the OEM creates an OS image customized with exactly the components needed for the target device. 
The process and tools by which customized images can be created is described here [IoT Core Manufacturing Guide](https://docs.microsoft.com/en-us/windows-hardware/manufacture/iot/iot-core-manufacturing-guide).

To enable the Azure Device Agent in such images, 

- Build the binaries
- Update the image manifst to:
    - Include the binaries
    - Configure the service
- Build the image

## Building the Binaries

- Follow the instructions [here](development-scenario/repo-build.md).

## Including the Binaries

- OEM authors packages to include all the necessary binaries. For example:

<pre>
    &lt;files&gt;
    &lt;file source="Limpet.exe" destinationDir="$(runtime.system32)" /&gt;

    &lt;file Source="AzureDeviceManagementClient.exe" destinationDir="$(runtime.bootDrive)\dm" /&gt;
    &lt;file Source="AzureDeviceManagementClient.json" destinationDir="$(runtime.bootDrive)\dm" /&gt;
    &lt;file Source="DeviceInfoPlugin.dll" destinationDir="$(runtime.bootDrive)\dm" /&gt;
    &lt;file Source="RebootPlugin.dll" destinationDir="$(runtime.bootDrive)\dm" /&gt;
    &lt;file Source="DeviceInfoManifest.json" destinationDir="$(runtime.bootDrive)\dm\manifests" /&gt;
    &lt;file Source="RebootManifest.json" destinationDir="$(runtime.bootDrive)\dm\manifests" /&gt;

    &lt;file Source="DMSetup.cmd" destinationDir="$(runtime.bootDrive)\OEMInstall" /&gt;
    &lt;/files&gt;
</pre>

## Configuring the Binaries

- To configure the `AzureDeviceManagementClient` service, create a cmd file and invoke it from the main configuration script `OEMCustomization.cmd` (which is called on every boot).

## Configuring the UWP Bridge

- The device management library used by the UWP application communicates with the NT Service, AzureDeviceManagementClient.exe, over a capability-protected RPC channel.  By default, this is configured to use the systemManagement 
capability.  Meaning that any UWP app using systemManagement can utilize this RPC channel.  If you want to further lock this down, you can request a Custom Capability from the store which 
will allow you to further secure this channel.  More information about Custom Capabilities can be found [here](https://github.com/Microsoft/Windows-universal-samples/tree/master/Samples/CustomCapability).

(see more samples on authoring OEMCustomization.cmd [here](https://github.com/ms-iot/iot-adk-addonkit/tree/master/Workspace/Source-arm/Products))

## Device Time

In order for the device agent to connect to the Azure IoT Hub, it is necessary that the device's time is in sync with the internet time.

To force a time sync, a recurring task can execute the following snippet on startup:

<pre>
    REM Force time sync on boot
	w32tm /resync /force
</pre>

## Sample Scripts

#### DMSetup.cmd

<pre>
    c:\dm\AzureDeviceManagementClient.exe -install
    c:\windows\system32\sc.exe config AzureDeviceManagementClient start=auto
    c:\windows\system32\sc.exe failure AzureDeviceManagementClient reset= 0 actions= restart/0/restart/0/restart/0
    net start AzureDeviceManagementClient
</pre>

#### OEMCustomization.cmd

<pre>
@echo off 
REM OEM Customization Script file 
REM This script if included in the image, is called everytime the system boots. 

reg query HKLM\Software\IoT /v FirstBootDone >nul 2>&1 

if %errorlevel% == 1 ( 
     REM Enable Administrator User 
     net user Administrator p@ssw0rd /active:yes 
     call c:\OEMInstall\DMSetup.cmd

     REM Resync with time server every hour	
     schtasks /Create /SC HOURLY /TN TimeSyncEveryHour /TR "w32tm /resync /force" /RU "SYSTEM"

     reg add HKLM\Software\IoT /v FirstBootDone /t REG_DWORD /d 1 /f >nul 2>&1 
) 

REM Force time sync on boot
w32tm /resync /force
</pre>

- It is advisable to set the correct timezone for the device as well, since Windows IoT Core defaults to US Pacific timezone, for example:

<pre>
     tzutil /s "W. Europe Standard Time"
</pre>

----

[Home Page](../../README.md) | [Device Agent](device-agent.md)
