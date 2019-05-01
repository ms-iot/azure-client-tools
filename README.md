# Azure Client Tools

### Overview

The Azure Client Tools is a collection of open-source libraries and executables that enable OEMs to rapidly build a device provisioning and management client that can be managed remotely through various Azure IoT services.

The code shared on this repo can be used as-is, or if desired, the OEM is free to modify or extend it to meet their specific needs.

Below is an overview of what each tool offers and links for more details if needed.

### Azure IoT Device Agent

The Azure IoT Device Agent is a service that enables the remote management of various platform components through Azure IoT Hub. It can be easily extended to enable more platform or OEM's components to be remotely managed too.

For more details, visit the [Azure IoT Device Agent](docs/device-agent/device-agent.md) page.

### DPS Client APIs

The DPS Client APIs are a set of C APIs that allow a device client to provision a device with an Azure IoT Hub identity using the [Azure Device Provisioning Service](https://docs.microsoft.com/en-us/azure/iot-dps/about-iot-dps).

These APIs enable the creation of custom DPS clients or the integration of such functionality into other existing components on the system.

Note that this repo provides the [Azure IoT Device Agent](docs/device-agent/device-agent.md), which has an integrated DPS client and can be used as-is without writing any new code (i.e. without having to learn the DPS Client APIs).
Only consider writing your own DPS client if the Device Agent provisioning does not meet your needs.

For more details, visit the [DPS Client API Documentation page](docs/dps-lib/dpsclientapis.md).

### Limpet

Limpet.exe allows local processess to use TPM for storing azure connection strings, retrieve SAS tokens, register the device with IoTHub using azure DPS service and many more.

For more details, visit the [Limpet page](docs/limpet/limpet.md).
