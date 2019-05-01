# Shutdown Extension

In this scenario, we will extend the device agent to support a remote command; the shutdown command.

Extension is done by writing a new plug-in, and deploying it to the device, and re-starting the device agent.

1. Configure Azure IoT Hub [here](configure-azure-iothub.md).
2. Note the connection string of the device you have created in the previous step.
3. Flash your Raspberry Pi device with RS5 IoT Core.
4. Follow the steps in the [Developer Scenario](../development-scenario.md).
5. Implement the invoke method such that it launches the `shutdown.exe` process with `-s -t 0`.
6. Open [https://portal.azure.com](https://portal.azure.com) in a browser.
7. Navigate to your IoT Hub, and then your device.
8. Click `Direct method` at the top bar.
9. Fill the fields:
  - Set `Method Name` to `shutdownCmd` (or the name of the command you have created).
  - Click `Invoke Method`.

----

[Quick Starts](../quick-start.md)