# Device Explorer

- Open DeviceExplorer (see instructions on [Setting up the developer machine](../development-scenario/developer-machine-setup.md)).
- Insert your IoT Hub Owner connection string. To retrieve the connection string:
    - Go to [portal.azure.com](http://portal.azure.com).
    - Browse to your your IoT Hub (search by name).
    - Under `Settings`, click `Shared access policies`.
    - Click `iothubowner`.
    - Click `Connection string-primary key`.
    - Copy its value.
- Create a new device if you haven't already (under the `Management` tab).
- Select the newly created device, and click `Twin Props.`.
- Send any of the snippets listed on [docs\schema\full-schema.md](../schema/full-schema.md) under 'Configuration Groups'
- Refresh the device twin in DeviceExplorer to see the responses from the device.

----

[Reference](../reference.md)
