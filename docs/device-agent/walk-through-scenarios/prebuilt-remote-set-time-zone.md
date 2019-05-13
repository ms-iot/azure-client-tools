# Set Time Zone Remotely

In this scenario, we will use IoT Hub service, and a pre-built set of binaries for the agent on the device.

1. Configure Azure IoT Hub [here](configure-azure-iothub.md).
2. Note the connection string of the device you have created in the previous step.
3. Flash your Raspberry Pi device with RS5 IoT Core.
4. Follow the instructions in our [Quick Start without Azure DPS](../quick-start-without-dps.md) guide to install and start the Agent.

The Device Agent should be running now. Time to invoke the reboot remotely...

1. Open [https://portal.azure.com](https://portal.azure.com) in a browser.
2. Navigate to your IoT Hub, and then your device.
3. Click `Device twin` at the top bar.
4. Change time zone to Mountain by pasting the following json snippet right after `"desired": {`
    <pre>
    "desired": {
         "timeInfo": {
              "__meta": {
                "deploymentId": "time_info_v1",
                "serviceInterfaceVersion": "1.0.0"
              },
              "ntpServer": "time.windows.com",
              "dynamicDaylightTimeDisabled": false,
              "timeZoneKeyName": "Mountain Standard Time",
              "timeZoneBias": 420,
              "timeZoneDaylightBias": -60,
              "timeZoneDaylightDate": "0000-03-02T02:00:00",
              "timeZoneDaylightName": "Mountain Daylight Time",
              "timeZoneDaylightDayOfWeek": 0,
              "timeZoneStandardBias": 0,
              "timeZoneStandardDate": "0000-11-01T02:00:00",
              "timeZoneStandardName": "Mountain Standard Time",
              "timeZoneStandardDayOfWeek": 0
         }
    }
    </pre>
5. Click `Save`.
6. Verify the clock has changed on the device.
7. Change time zone back (for example, to Pacific) by replacing the previous json snippet with the following:
    <pre>
    "desired": {
          "timeInfo": {
                "__meta": {
                    "deploymentId": "time_info_v2",
                    "serviceInterfaceVersion": "1.0.0"
                },
               "ntpServer": "time.windows.com",
               "dynamicDaylightTimeDisabled": false,
               "timeZoneKeyName": "Pacific Standard Time",
               "timeZoneBias": 480,
               "timeZoneDaylightBias": -60,
               "timeZoneDaylightDate": "0000-03-02T02:00:00",
               "timeZoneDaylightName": "Pacific Daylight Time",
                "timeZoneDaylightDayOfWeek": 0,
               "timeZoneStandardBias": 0,
               "timeZoneStandardDate": "0000-11-01T02:00:00",
               "timeZoneStandardName": "Pacific Standard Time",
               "timeZoneStandardDayOfWeek": 0
          }
    }
    </pre>
8. Click `Save`.
9. Verify the close has changed on the device.

Coverage: IoT Core/ARM/Quick Start

----

[Quick Starts](../quick-start.md)