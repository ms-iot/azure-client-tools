# Developer Install

## Installing on IoT Core

- Create a file layout. Make sure *the architecture matches* the target OS. WoW support does not exist on all platforms.

    <pre>
    scripts\deploy.create-device-folder.cmd x64 Debug c:\deploy
    </pre>

- Insert your connection string in `c:\deploy\bin\AzureDeviceManagementClient.json`.
    <pre>
      "debugConnectionString": "<i>&lt;insert your connection string here&gt;</i>"
    </pre>
    **Note**: This will only work for debug builds by design. Retail images should not read the connection string from an unencrypted text file.

- Copy the generated layout to the test device  (i.e. copy c:\deploy to the device).
- Register the service:
    <pre>
    <i>&lt;target folder on device&gt;</i>\bin\AzureDeviceManagementClient.exe -install
    </pre>

    - **Note**: You need to run the `-install` step only once on a clean machine or when you register the exe from a new location (after -uninstall).

- Start the service:
    <pre>
    net start AzureDeviceManagementClient
    </pre>

    - You can also start the client as a regular (non-admin) process with console output:
    <pre>
    AzureDeviceManagementClient -debug
    </pre>

## Installing on IoT Enterprise

- Enable [Embedded Mode](https://docs.microsoft.com/en-us/windows/iot-core/develop-your-app/embeddedmode).
- Follow the same steps as those for IoT Core.

----

[Development Scenario](../development-scenario.md)