# Migration from Old MSIoT DM Client

## Overview

In October 2017, we made `iot-core-azure-dm-client` available to the public. At the time, Azure IoT Hub allowed only a single identity per device - and hence a single connection.
This means that if there are two applications running on the device, they have to either:

1. Share the same connection to IoT Hub.
2. Have separate device identities in IoT Hub and consequently can have separate connections to IoT Hub.

From the operator perspective, it was desired that both applications appear in the same 'device' - not as two separate devices. So, this requirement eliminated option #2 above.

To achieve #1, we opted for having the OEM's UWP application own the connection to IoT Hub, and share it with the device management service through a library (DMClientLib) that facilitated cross process communication.

While this worked for many customers, we always wanted to make the device management service independent from any other application on the system given its critical role and need for resiliency.

With the introduction of [IoT Hub models](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-module-twins), a new path opened where two connections can be established from the same device, and still both will be associated with the same device in the cloud.

Taking advantage of IoT Hub modules, we re-built the device management client from the ground up again to be completely independent of any other application.

As we migrated much of the older device management service into the new one, we made minor changes to the schema, along with a new error reporting format, and the addition of a summary section.

If you have a cloud solution that already talks to the older client, it will need to be updated to write and read the new schemas. Again, the changes in the schemas are not major.

## Twin Assignment

- The device management client can be configured to use either a device twin or a module twin. For more details, see 'Device Twin Modules' in the [agent configuration](reference/device-agent-configuration-file.md) page.

## Json Interface

Below is a list of the changes:

- All configurations are no longer under `Windows`.
    - This is possible now since the entire twin is owned by the device management and is not shared with other applications.
    - This saves us also one nesting level (since the twin offers a maximum of 5 levels).

- There is a new error schema.
    - See the definition [here](schema/configuration-groups/error-object.md).

## UWP Interface

This is not restricted to local access to device management functionality.
For example, if a UWP application needs to reboot the device, it can connect to the device management service and issue a reboot command.
This is done through the introduction of a WinRT library for UWP applications to use.

For more details, see our [UWP Bridge](uwp-bridge.md) page.

----

[Home](../../README.md) | [Device Agent](device-agent.md)
