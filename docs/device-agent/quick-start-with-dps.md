# Quick Start with Azure DPS

In this tutorial, we will walk the reader through the minimal steps to get a debug version of the Device Agent running on a Windows IoT Core device.

For a quick start on Windows IoT Enterprise, see this [page](quick-start-without-dps.md).

We will be using the Windows Device Portal (WDP) to configure the device remotely.
WDP comes with all our Windows IoT Core pre-built images, and can be accessed by simply typing the device ip followed by :8080.
WDP is a development tool and should not be part of your production image.
More information about WDP can be found [here](https://docs.microsoft.com/en-us/windows/uwp/debug-test-perf/device-portal).

We will also be using the Azure Device Provisioning Service (DPS). This service allows the device to get its IoT Hub identity after its image has been built.
This is achieved by harvesting an offline device identity (TPM endorsement key), and registering that information with the DPS. When the device is later deployed, and it connects to the cloud, it will use that identity with DPS to retrieve its IoT Hub identity.
More information about DPS can be found [here](https://docs.microsoft.com/en-us/azure/iot-dps/).

Here's an overview of the steps:
- Verify TPM compatibility
- Get the device identity information.
- Create the Device Provisioning Service and enroll the device.
- Configure the device and start the Device Agent.
    - Device with Only the Device Agent
    - Device with the Device Agent and Two Other IoT Hub Applications

## Verify TPM Compatibility

See our [TPM Compatibility for DPS Scenarios](tpm/tpm-dps-compatibility.md) page.

If your device is not compatible, you can install a TPM simulator to test out the scenarios.
However, TPM simulators are not secure and should not be used in a production environment. See our [Installing TPM Simulator](tpm/tpm-simulator-install.md) page.

## Get Device Identity Information

In this part, we harvest the information that identifies the device to the Azure device provisioning service.

1. Get Limpet.exe (see the [Installing Pre-built Binaries](install-pre-built-binaries.md) page).

2. Connect to the device and run the following commands:
    <pre>
    cd c:\windows\system32
    limpet.exe -azuredps -enrollmentinfo
    </pre>

    The output should look something like this:

    <pre>
    Provisioning information

        Registration Id:
        &lt;registration_id&gt;

        Endorsement Key:
        &lt;endoresement_key&gt;
    </pre>
- Note both the &lt;registration_id&gt; and the &lt;endoresement_key&gt; keys.

## Create the Device Provisioning Service

In this part, we configure the Azure device provisioning service to be ready to re-direct our device when it starts running device management.

- Navigate to [http://portal.azure.com](http://portal.azure.com)
- On the left pane, click 'Create a resource'.
- Search for 'IoT Hub Device Provisioning Service', create one, and navigate to it.
- On the left, click on `Linked IoT hubs` and then `Add`. Select the IoT Hub you want the devices to be created at.
- Click 'Manage enrollments' on the left, and then click 'Add individual enrollment'.
- In the 'Add Enrollment' page, fill the fields as follows:
    - Set 'Mechanism' to 'TPM'.
    - Set 'Endorsement key' to the &lt;endoresement_key&gt; value captured in the previous step.
    - Set 'Registration ID' to the &lt;registration_id&gt; value captured in the previous step.
    - Set 'IoT Hub Device ID' to the name you would like the device to have in IoT Hub.
    - Click 'Link to IoT Hub', and select an IoT Hub target for the device to be created.
    - Click 'Save' at the top of the page.
- Go back to the 'Overview' tab of your DPS instance.
    - Note down the 'ID Scope'.

## Configure the Device and Start the Device Agent.

### Device with Only the Device Agent 

- Navigate to the device portal (WDP) using your browser (by typing http://&lt;ip&gt;:8080).
- On the left pane, click 'Azure Clients'
    - The 'Azure Clients' page shows the current state of the device provisioning parameters and the device management client.
    - Under the 'Device Provisioning Client' section, 
        - Set the 'Id Scope' to the value you noted from the previous step.
        - Set the 'Logical Device Id' to 0.
        - Click 'Configure'.
            - This will write those values to the device so that when the device management client starts, it can contact the DPS and provision the device.
    - Under the 'Device Management Client' section,
        - Install the latest tools.
            - The tools will be installed to `c:\dm` on the device.
            - Logs will also be created in the same location.
        - Start the device management client by selecting `Start` from the `Actions` combobox.

You can now use your [Azure Portal](https://portal.azure.com) or [Azure Device Explorer](https://github.com/Azure/azure-iot-sdk-csharp/tree/master/tools/DeviceExplorer) to start interacting with the device and access many of its capabilities.

Note that the device client reports to the twin a list of all the available capabilities.

### Device with the Device Agent and Two Other IoT Hub Applications

#### Overview

To be able to provision multiple applications on the device, and associate them to the same IoT Hub device, we need to use sub-device IoT Hub identities - namely [Module Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-module-twins).

Using module twins allows the device developer to avoid representing two applications (running on the same physical device) as two separate device identities in IoT Hub.

The Device Agent enables its users to make use of module twins to provision all applications on the device.
The device builder has to describe what device and sub-device identities are needed, where they need to be stored in the TPM, and the Device Agent will take care of that.
The applications just need to wait for their connection strings to appear in the TPM, and once its been placed there by the Device Agent, they can proceed to connect.

The DPS provisioning scenario is as follows:

- DPS provisioning creates the device and its device twin and retrieves the device connection string.
- The Device Agent uses the device connection string to create the other modules based on its configuration file.
- The Device Agent stores the connection strings for the newly created modules in their designated TPM slots.
- Applications wait until their connection strings appear in the TPM and then reads them from there.

Note the following considerations:

- The device connection string is not optional and has to be stored in TPM slot 0 - this cannot be changed.
- The Device Agent connection string can be either:
    - The device connection string.
        - Set the `dmModuleSlotNumber` to the same TPM slot as the device connection string (i.e. 0).
    - A module connection string.
        - Set the `dmModuleSlotNumber` to a TPM slot number different from 0. This will cause the Device Agent to create a module for itself and store its connection string in the designated TPM slot.
- An application connection string can be either:
    - The device connection string.
        - You just need to make sure the Device Agent (or any other application) is not using TPM slot 0.
    - A module connection string.
        - Add a new module name and TPM slot number mapping under the `otherModuleIds` node.

#### Example and Walk-Through

For example, let's say we have two applications and the Device Agent - and we want the following configuration:
- Application A will use the device twin connection string and it will pick it up from TPM slot 0.
- The Device Agent will use a module twin connection string and it will pick it up from TPM slot 1.
- Application B will use a module twin connection string and it will pick it up from TPM slot 2.

The above configuration can be expressed as follows:

<pre>
  ...
  "dpsEnabled": true,
  "dpsUri": "global.azure-devices-provisioning.net",
  "dpsScopeId": "0ne000567F7",
  "deviceSlotNumber": 0,
  "dmModuleSlotNumber": 1,
  "dmModuleId": "dmModule",
  "otherModuleIds": {
    "AppB": 2
  },
  ...
</pre>

The applications A and B need to be built in such a way to keep trying to acquire the connection string from their designated TPM slot. That way, when the connection string is placed there by the Device Agent, they will be able to pick it up.

Now, the steps to configure this:

- In your browser, navigate to the device portal (WDP) using your browser (by typing http://&lt;ip&gt;:8080).
    - On the left pane, click 'Azure Clients'
        - The 'Azure Clients' page shows the current state of the device provisioning parameters and the device management client.
        - Under the 'Device Management Client' section,
            - Install the latest tools.
                - The tools will be installed to `c:\dm` on the device.
                - Logs will also be created in the same location.
- From your desktop,
    - Navigate to the device `\\ip\c$` and enter the user name and password.
    - Open `\\ip\c$\dm\AzureDeviceManagementClient.json` in notepad or any text editor.
    - Make sure the above json attributes are preset and set to the values indicated above.
    - Save and close.
- Back to your browser, and under the 'Device Management Client' section:
    - Start the device management client by selecting `Start` from the `Actions` combobox.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Quick Starts](quick-start.md)
