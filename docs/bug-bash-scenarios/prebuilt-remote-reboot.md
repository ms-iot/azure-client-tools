# Reboot a Device Remotely

In this scenario, we will use Azure device provisioning service, IoT Hub service, and a pre-built set of binaries for the agent on the device.

1. Configure Azure Device Provisioning Service [here](configure-azure-dps.md).
2. Configure Azure IoT Hub [here](configure-azure-iothub.md).
3. Flash your **SnapDragon** device with RS5 IoT Core.
4. Follow the instructions in our [Quick Start](../device-agent/quick-start-with-dps.md) guide to install and start the Agent.

The Device Agent should be running now. Time to invoke the reboot remotely...

1. Open [https://portal.azure.com](https://portal.azure.com) in a browser.
2. Navigate to your IoT Hub, and then your device.
3. Click `Direct method` at the top bar.
4. Fill the fields:
  - Set `Method Name` to `StartRebootCmd`.
  - Click `Invoke Method`.

Coverage: IoT/Core/ARM/DPS/Quick Start

----

[Bug Bash Scenarios](bug-bash-scenarios.md)