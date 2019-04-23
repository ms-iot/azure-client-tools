# Supported SKUs

The Azure Device Agent is supported on:

- Windows IoT Core
- Windows IoT Enterprise

While all the Azure Device Agent functionality is enabled by default on Windows IoT Core, some extra steps might be needed to enable the same functionality on Windows IoT Enterprise.

## Embedded Mode

Some of the provided plug-ins (like device info, reboot, etc) rely on Windows [CSPs](https://docs.microsoft.com/en-us/windows/client-management/mdm/configuration-service-provider-reference) to apply/retrieve configurations.

Those CSPs cannot be accessed except when Embedded Mode is enabled. Embedded Mode is enabled by default on Windows IoT Core - however, on Windows IoT Enterprise, it must be enable for those plug-ins to work.

Embedded Mode can be enabled by creating a provisioning package and applying it to the Windows IoT Enterprise image. Detailed steps on how to create the provisioning package can be found on the [Embedded Mode](https://docs.microsoft.com/en-us/windows/iot-core/develop-your-app/embeddedmode) page.

For testing, you can just double click and install the generated provisioning package on the Windows Iot Enterprise SKU to enable embedded mode.

----

[Home](../../README.md) | [Device Agent](device-agent.md)
