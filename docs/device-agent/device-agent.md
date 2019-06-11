# Azure IoT Device Agent

## Quick Links

The device agent is shared as an open source project in this repo.

- [Quick Starts](quick-start.md)
- [Development Scenario Walk-through](development-scenario.md)
- [QR Walk-through](walk-through-scenarios/qr-provisioning.md)
- [Device Agent Reference](reference.md)

## Overview

Microsoft Azure provides scalable and secure cloud services that enable device management for a large number of devices.
The services act as the cloud infrastructure connecting operators to devices.

<img src="high-level-e2e.png"/>

As shown in the diagram, to have an end-to-end solution, the following components need to be present:
1. A cloud infrastructure.
2. A device agent to connect various components on the physical device to the cloud services.
3. A cloud solution to present the various cloud services in a format suitable for the operator scenario.

## The Cloud Infrastructure

The cloud infrastructure is provided by Azure. You can read about the various services here:
- [Azure IoT Hub](https://docs.microsoft.com/en-us/azure/iot-hub/)
- [Azure Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins)
- [Azure Device Provisioning Service](https://docs.microsoft.com/en-us/azure/iot-dps/)
- [Azure Storage](https://azure.microsoft.com/en-us/services/storage/)

## The Device Agent and the Cloud Solution

Both the device agent and the cloud solution can be implemented by using the Azure SDKs provided for the above mentioned services. However, given the commonality of many of their functions, there are multiple offerings that can give a head start.

### The Cloud Solution

The Cloud Solution can be built using the [Azure SDKs](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-sdks) or an existing solution by Microsoft or a third party can be adopted and extended to meet the specific needs and scenarios of the operator.

Microsoft provides two options:
- [Azure IoT Central](https://azure.microsoft.com/en-us/services/iot-central/)
- [Azure Solution Accelerators](https://azure.microsoft.com/en-us/features/iot-accelerators/)

### The Device Agent

For the device agent, Microsoft provides a ready-to-build open source solution that:

- Manages the device identity provisioning with IoT Hub.
- Manages the cloud connection and its renewal.
- Provides a plug-in model for platform components which allows easy onboarding to various Azure services. This model includes discovery, initialization, error reporting, and state aggregation.
- Comes with a set of ready-to-ship plug-ins for very commonly used platform components.

#### Supported SKUs

The device agent can run on both the Windows IoT Core SKU and the Windows IoT Enterprise SKU. More information can be found on the [Supported SKUs](supported-skus.md) page.

#### Get Started

The device agent is shared as an open source project in this repo.

- [Quick Starts](quick-start.md)
- [Development Scenario Walk-through](development-scenario.md)
- [Device Agent Reference](reference.md)

#### Servicing

See the [Servicing](servicing.md) page.

## The Device Agent and the Edge Runtime

The Device Agent is designed so that it can co-operate with other applications running on the device that need a connection to IoT Hub.

The Device Agent can be assigned the role of provisioning the device for all applications (by retrieving/generating connection strings for all of them). Or, it can be told to just wait for the device to be provisioned - which allows other provisioning clients to be used.

If a provisioning client other than the integrated one with the device agent is to be used, the provisioning client must be able to assign multiple connection strings to different slots in the TPM, so that the device builder can designate which slot the device agent needs to read its connection string from as well as which slots the other applications will read from.

If DPS provisioning is to be used, 
- The Edge Runtime has its own integrated client and it support the above requirement (i.e. cannot provision other applications).
- The device agent has its own integrated client and it meets the requirements, however, the Edge Runtime does not support waiting for another provisioning client.

As a result, DPS provisioning for a device that has both the device agent and the Edge Runtime is not currently supported. We are working with the Edge Runtime team on supporting this scenario.

## Migrating from `iot-core-azure-dm-client`

If you are using the device management client `iot-core-azure-dm-client` and would like to read how it relates to the new client and how to migrate, check out [Migration from Old MSIoT DM Client](migration-from-old-client.md) page.

----

[Home](../../README.md)
