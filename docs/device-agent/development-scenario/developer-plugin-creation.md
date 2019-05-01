# Authoring New Plugins

## Overview

Plug-ins can be authored by any developer. Typically, a new plug-in will be authored in its own repo/solution or as part of the OEM's code base.

Microsoft provides some of the most commonly used plug-ins as part of this repo/solution.

## Creating the Skeleton

- [Create a New Plugin as a Standalone Project](plugin-build-standalone.md)
- [Create a New Plugin to The Agent Solution](plugin-build-with-agent.md)

## Defining the Interface

- [Defining a New Json Interface](../schema.md)

## Implementing The Handler

- [Implementing Json Interfaces](plugin-raw-code.md)

## Creating the Plugin Manifest

- Add a new file 'componentNamePlugin.json' ([Sample](TemplateFiles/TemperatureSensorPlugin.json)).

## Adding Handler Specific Configuration

- Open the agent configuration file.
    - A sample can be found here: [AzureDeviceManagementClient.json](../../../code/device-agent/agent/AzureDeviceManagementClient.json).
- Add a new `<handler_id>` node under `handlers`.
    - The value of that node will be passed as is to the `Start()` method every time it is called.
    - `<handler_id>` is the string you pass to the `BaseHandler` constructor as the handler's id.
    <pre>
    {
      "handlers": {
        "componentName": {
          "reportOnConnect": true
        }
      }
    }
    </pre>

References
- [Device Agent Configuration File](../reference/device-agent-configuration-file.md).

## Deploying

- Copy the manifest to where your AzureDeviceManagementClient.json specifies.
- Copy the dll to where the manifest specifies.


## Testing

- [Test The Json Interface](plugin-raw-test.md)

## Debugging Plugin Code

It is easier to debug in-proc plug-ins than out-of-proc plug-ins. The good news is that you can switch between both by setting the `outOfProc` attribute in the plug-in manifest to true/false.
This is specially useful when debugging start-up code.

To debug load issues:

- Options 1:
    - Check the verbose logging of the enumeration code. You can enable verbose logging by following the steps described in the [Debugging](debugging.md) page.
- Option 2:
    - Set a break point at `AzureDMClient::EnumeratePlugins`.
    - Start the `AzureDeviceManagementClient` under the debugger (enumeration takes place at the very beginning and there will be no time to attach).

----

## Related Topics

- [Extensibility and Discovery Model](extensibility-and-discovery-model.md).
- [Handler Enumeration and Initialization Flow](extensibility-and-discovery-model/handler-enumeration-flow.md).
- [Debugging](debugging.md).

----

[Home](../../../README.md) | [Device Agent](../device-agent.md) | [Development Scenario](../development-scenario.md)
