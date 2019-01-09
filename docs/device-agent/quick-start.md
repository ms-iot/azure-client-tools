# Quick Start

In this tutorial, we will walk the reader through the minimal steps to get a debug version of the Device Agent running on their device.

We will be using the Windows Device Portal (WDP) to configure the device remotely.
WDP comes with all our Windows IoT Core pre-built images, and can be accessed by simply typing the device ip followed by :8080.
WDP is a development tool and should not be part of your production image.
More information about WDP can be found [here](https://docs.microsoft.com/en-us/windows/uwp/debug-test-perf/device-portal).

We will also be using the Azure Device Provisioning Service (DPS). This service allows the device to get its IoT Hub identity after its image has been built.
This is achieved by harvesting an offline device identity (TPM endorsement key or an x509 certificate), and registering that information with the DPS. When the device is later deployed, and it connects to the cloud, it will use that identity with DPS to retrieve its IoT Hub identity.
More information about DPS can be found [here](https://docs.microsoft.com/en-us/azure/iot-dps/).

Here's an overview of the steps:
- Get the device identity information.
- Create the Device Provisioning Service and enroll the device.
- Configure the device and start the Device Agent.

## Get Device Identity Information

In this part, we harvest the information that identifies the device to the Azure device provisioning service.

- Make sure the test device supports TPM 2.0 (for example, a SnapDragon 410c).
- Navigate to the device portal (WDP) using your browser (by typing http://&lt;ip&gt;:8080).
- On the left pane, click 'TPM Configuration'
    - The 'TPM Configuration' page shows whether the TPM Tools are Installed or not. Make sure the latest tools are installed by clicking "Install Latest". Limpet.exe will be installed to `c:\windows\system32`.
    - Once the TPM Tools are installed, make sure Slot 0 in the TPM is not provisioned (under "Logical Devices Setttings").
- Connect to the device and run the following commands:
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
- Make sure the 'Global device endpoint' is set to 'global.azure-devices-provisioning.net'.
- Click 'Manage enrollments' on the left, and then click 'Add individual enrollment'.
- In the 'Add Enrollment' page, fill the fields as follows:
    - Set 'Mechanism' to 'TPM'.
    - Set 'Endorsement key' to the &lt;endoresement_key&gt value captured in the previous step.
    - Set 'Registration ID' to the &lt;registration_id&gt; value captured in the previous step.
    - Set 'IoT Hub Device ID' to the name you would like the device to have in IoT Hub.
    - Click 'Link to IoT Hub', and select an IoT Hub target for the device to be created.
    - Click 'Save' at the top of the page.
- Go back to the 'Overview' tab of your DPS instance.
    - Note down the 'ID Scope'.

## Configure the Device and Start the Device Agent.

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

You can now use your Azure portal or Azure Device Explorer to start interacting with the device and access many of its capabilities.

Note that the device client reports to the twin a list of all the available capabilities.

----

[Device Agent](device-agent.md) | [Home](../../README.md)
