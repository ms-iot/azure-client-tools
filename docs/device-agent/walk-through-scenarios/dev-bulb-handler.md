# Bulb Extension

In this scenario, we will extend the device agent to support configuring the state of a bulb remotely (on/off).

Extension is done by writing a new plug-in, and deploying it to the device, and re-starting the device agent.

1. Configure Azure IoT Hub [here](configure-azure-iothub.md).
2. Note the connection string of the device you have created in the previous step.
3. Flash your Raspberry Pi device with RS5 IoT Core.
4. Follow the steps in the [Developer Scenario](../development-scenario.md).
5. Implement the invoke method such that
   - It parses the incoming json according to the schema you have defined and takes the appropriate action.
   - It reports the current state of the bulb.
6. Open [https://portal.azure.com](https://portal.azure.com) in a browser.
7. Navigate to your IoT Hub, and then your device.
8. Click `Device twin` at the top bar.
9. Under "desired", type in the new json schema your new handler understands.

----

[Quick Starts](../quick-start.md)