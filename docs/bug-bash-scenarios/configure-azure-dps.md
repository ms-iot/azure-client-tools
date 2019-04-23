# Configure Azure Device Provisioning Service

Azure DPS service delivers a connection string to the device based on its secure hardware identity.

The secure hardware identity is obtained from its TPM. If the device does not have a TPM, the Device Agent cannot be used with DPS (support is coming later).

To configure your Azure Device Provisioning Service:

1. [https://portal.azure.com]
2. Use your Microsoft credentials.
3. Click `Create a resource`.
4. Type `IoT Hub Device Provisioning Service` in the search box.
5. Click the `Create` button.
6. Fill out the fields:
  - Type in a name with your alias in it.
  - Choose the IOT-Athens-SLFHST subscription if available. If not, use the your MSDN subscription.
  - If you don't already have a resource group, create one.
  - Choose `West US`.
  - Click `Create`.
7. Click on the newly created DPS instance, and note the `ID Scope` string. We will use it in configuring the device later.

To verify the successfull creation,
- Click on `Resource groups` on the left.
- Type part of the resource group name you've chosen in the search box.
- Click the resource group.
- Click the the DPS you have just created.

----

[Bug Bash Scenarios](bug-bash-scenarios.md)