# Configure Azure IoT Hub Service

Azure IoT Hub service provides the following:
- A store for each device configuration, along with notifications when values changes.
- A mechanism to invoke methods on the device.
- An event hub where telemetry messages from devices can be monitored/received.

In order for a device to connect to Azure IoT Hub, Azure IoT Hub has to have a virtual device created first, and its connection string needs to be stored on the device securily so that an application can use it to connect to the IoT Hub service (as a client).

The virtual device can be created manually or it can be created during the device provisioning using the [DPS service](configure-azure-dps.md).

A virtual device consists of:
- An identity
    - Used to identify traffic to and from the physical device.
- A connection string to allow a signle process to connect to it.
- A device twin to store the configuration.
- 0 or more module twins to store additional configuration.

To configure your Azure IoT Hub service:

1. [https://portal.azure.com]
2. Use your Microsoft credentials.
3. Click `Create a resource`.
4. Type `IoT Hub` in the search box.
5. Click the `Create` button.
6. Fill out the fields:
  - Choose the IOT-Athens-SLFHST subscription if available. If not, use the your MSDN subscription.
  - If you don't already have a resource group, create one.
  - Choose `West US`.
  - Type in a name with your alias in it.
  - Click `Review + Create`.
  - Click `Create`.

To verify the successfull creation,
- Click on `Resource groups` on the left.
- Type part of the resource group name you've chosen in the search box.
- Click the resource group.
- Click the the IoT Hub you have just created.

To manually create a virtual device in IoT Hub,
- Click on the IoT Hub you have just created.
- Click `IoT devices` on the left pane.
- Click `Add` at the top.
- Fill out the fields:
  - Type a device id of your choice.
  - Make sure `Symmetric key` is selected.
  - Click `Save`.

To explore the contents of the device Device Twin:
- Click on the IoT Hub you have just created.
- Click `IoT devices` on the left pane.
- Click on the device you have just created.
- At top, click `Device twin`.
- Note that you can also invoke a `Direct method` from the same place.

----

[Bug Bash Scenarios](bug-bash-scenarios.md)