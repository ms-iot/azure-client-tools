# Executable commands
    
Used to register the azure device management service on the device. You need to run the `-install` step only once on a clean machine or when you register the exe from a new location (after -uninstall).
<pre>
AzureDeviceManagementClient -install
</pre>

Used to uninstall the azure device management service on the device
<pre>
AzureDeviceManagementClient -uninstall
</pre>

You can start the client as a regular (non-admin) process with console output:
<pre>
AzureDeviceManagementClient -debug
</pre>
