# Debugging

## ETL logs

For production environments, detailed ETL logging is supported. The provider guid to monitor is `e1688237-74f7-54f0-7ff7-9ba255fa157c`.

ETL logs can be turned on remotely using the Diagnostics Logs CSP or by running `xperf.exe` on the device.

`xperf.exe` comes with IoT Core. On desktop, `xperf.exe` can be installed with the Windows ADK.

Here's a sample on how to use xperf.exe to capture the logs:

<pre>
xperf.exe -start MySession -f c:\Temp\dm.etl -on e1688237-74f7-54f0-7ff7-9ba255fa157c

REM run you scenario now.

xperf.exe -stop MySession
</pre>

## Text Log Files

Text log files should be enabled only for debugging a specific failure or for development scenarios. They are not meant to be a production mechanism for monitoring. For production, use ETL (see above).

To enable text log files, 
- Create a json file `AzureDeviceManagementClient.json`.
- Place it in the same folder as `AzureDeviceManagementclient.exe`.
- Set the value of `textLogFilesPath` to the desired logs folder.

<pre>
{
  "textLogFilesPath": "c:\\temp\\dm\\logs",
  "pluginManifestsPath": "c:\\temp\\dm\\manifests",
}
</pre>

## Using the Debugger

#### As a regular exe

- Run `AzureDeviceManagementClient.exe -debug`.
  - Can be launched form the debugger of your choice.
  - Runs as the current user (not as the production environment's System account).
  - Convenient for starting under the debugger, and debugging scenarios that don't require elevation.

#### As an NT Service

After starting the service, attach the debugger.

If the service is running on the developer box, start Visual Studio as an administrator, and then attach.

If the service is running on a device,

- On the device, run:
    <pre>
    cdd windows\system32
    execd mwdbgsrv.exe -Argument "-t tcp:port=5000,IcfEnable"
    tlist
    </pre>

- Note that process id for AzureDeviceManagementClient.exe.

- On the developer machine, run:
<pre>
c:\debuggers\windbg -premote tcp:port=5000,server=deviceIP -p processID
</pre>

----

[Development Scenario](../development-scenario.md)