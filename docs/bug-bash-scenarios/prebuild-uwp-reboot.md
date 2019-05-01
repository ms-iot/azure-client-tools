# Reboot a Device from a UWP Application

In this scenario, we will use IoT Hub service, and a pre-built set of binaries for the agent on the device.

1. Configure Azure IoT Hub [here](configure-azure-iothub.md).
2. Note the connection string of the device you have created in the previous step.
3. Flash your Raspberry Pi device with RS5 IoT Core.
4. Follow the instructions in our [Quick Start without Azure DPS](../device-agent/quick-start-without-dps.md) guide to install and start the Agent.

The Device Agent should be running now. Time to create the UWP...

1. In Visual Studio, create a new UWP blank C# project. The target SDK version should be 16299.
2. Download and install the pre-built azure-device-agent-uwp-bridge-lib.2.0.0.nupkg from the [downloads folder](../../downloads).
    <pre>
    install-package azure-device-agent-uwp-bridge-lib
    </pre>
3. Modify the Package.appxmanifest:

    <pre>
        &lt;Package
          xmlns="http://schemas.microsoft.com/appx/manifest/foundation/windows10"
          xmlns:mp="http://schemas.microsoft.com/appx/2014/phone/manifest"
          xmlns:uap="http://schemas.microsoft.com/appx/manifest/uap/windows10"
          xmlns:iot="http://schemas.microsoft.com/appx/manifest/iot/windows10"
          IgnorableNamespaces="uap mp iot"&gt;
          .
          .
          .
        &lt;Capabilities&gt;
          &lt;Capability Name="internetClient" /&gt;
          &lt;iot:Capability Name="systemManagement" /&gt;
        &lt;/Capabilities&gt;
      &lt;/Package&gt;
    </pre>

4. Modify the MainPage.xaml and MainPage.xaml.cs
  - Add a button and a handler for click.
  - At the top of MainPage.xaml.cs, add:
    <pre>
      using DMBridgeComponent;
    </pre>
  - In the click handler, add:
    <pre>
        private void OnReboot(object sender, RoutedEventArgs e)
        {
            var reboot = new RebootBridge();
            reboot.Invoke();
        }
    </pre>

Time to invoke the reboot...

- Start the Device Agent (net start AzureDeviceManagementClient).
- Start the UWP application (to run on the same device as the Device Agent).
- Click the `Reboot` button.

It should reboot...

Coverage: ARM/UWP/Nuget/Quick Start

----

[Bug Bash Scenarios](bug-bash-scenarios.md)