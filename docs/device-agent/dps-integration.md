# Device Provisioning Service Integration

The Device Provisioning Service is an Azure service that allows the devices to receive their IoT Hub identities after they have been deployed to the field.
This is done by registering a device hardware identity (like the TPM Endorsement Key) or a certificate with Azure's Device Provisioning Service, and then having the device contact that service at some point in the future and identify itself, so that it received the IoT Hub device connection string in return.

If multiple applications on the device are to connect to Azure IoT Hub under the same device, each application needs to have a sub-device identity (so they retain their relationship to the device).
This is a requirement because no two applications can have the same identity. If that happens, whoever attempts to connect last wins, and the other is disconnected.

Sub-device identities are provided by Azure's IoT Hub through module twins. Module twins are exactly like device twin, only they have an association to a parent IoT Hub device.
Module twins have their own connection strings - and they can be created from the service side (through the Azure portal or through the Azure Service SDK), or from the device side using the device connection string .

The Device Agent can be configured to created module twins, derive their connection strings, and save them in designated TPM slots for applications to consume and connect to IoT Hub.

The image builder should identify which applications being packaged will need an Azure connection to IoT Hub, and then assigns each a module twin, and expresses that configuration in the [Device Agent Configuration File](reference/device-agent-configuration-file.md) to provision on start-up the first time.

The Device Agent is typically assigned a module twin rather than the device twin. This leaves the device twin to the main OEM applications if desired.
Of course, the image builder can still configure the Device Agent to use the device twin if wanted.
That way, the device management functionality can benefit from some of the Azure services that can target only device twins (but not module twins) - like ADM.

For details on how to configure DPS and do the assignment, see the [Device Agent Configuration File](reference/device-agent-configuration-file.md) page.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)