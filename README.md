# Azure Client Tools - Public Preview

### Overview

The Azure Client Tools is a collection of open-source libraries and executables that enable OEMs to rapidly build a device management client that can be managed remotely through various Azure IoT services.

The code shared on this repo can be used as-is, or if desired, the OEM is free to modify or extend it to meet their specific needs.

Below is an overview of what each tool offers and links for more details if needed.

### Azure IoT Device Agent

The Azure IoT Device Agent is a service that enables the remote management of various platform components through Azure IoT Hub. It can be easily extended to enable more platform or EOM's components to be remotely managed too.

For more details, visit the [Azure IoT Device Agent page](docs/device-agent/device-agent.md).

### Limpet

Limpet.exe allows local processess to use TPM for storing azure connection strings, retrieve SAS tokens, register the device with IoTHub using azure DPS service and many more.

For more details, visit the [Limpet page](docs/limpet/limpet.md).

### DPS Client APIs

The DPS Client APIs are a set of C APIs that allow a device client to provision a device with an Azure IoT Hub identity using the Azure Device Provisioning Service.

For more details, visit the [DPS Client API Documentation page](docs/dps-lib/dpsclientapis.md).
