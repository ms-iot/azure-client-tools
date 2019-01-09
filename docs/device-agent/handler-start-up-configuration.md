# Handler Start-up Configuration

## Overview

The handler start-up configuration provides a way for the image builder to configure a handler independent of the twin configuration.
This can be useful in two scenarios:

- Provide customization of the handler behavior at image build time.
- Provide defaults to properties that may be defined later by the twin.

## Authoring

The `"handlers"` node in the device agent configuration file holds a map of the handler identifier to the corresponding start-up configuration.

<pre>
{
    "handlers": {
        "&lt;handler-id&gt;": {
            "&lt;property0&gt;": "&lt;value0&gt;"
        }
    }
}
</pre>

The properties defined are completely up to the author of the handler. Each handler is expected to document what the image builder can configure.

## Common Properties

Even though the author of each handler can choose to define what properties the handler will honor, there is a set of common properties used.
The device agent does not enforce implementing them - but for consistency, it is highly recommended that they are implemented.

| Name | Type | Required | Description |
|----|----|----|----|
| `reportOnConnect` | boolean | No | If true, the handler should report the state during its `OnConnectionStatusChanged()` when the value is eOnline. <br/><br/>This is particularly useful for targeting specific devices based on their reported properties before they are configured. For example, a service can query IoT Hub for devices with architecture set to x86 to start targeting them with a certain configuration profile.<br/><br/>In that scenario, notice that before we configure the device, we want it to report its architecture. |
| `textLogFile` | string | No | For non-buildt-in handlers, it specifies where the text log file will be placed. If empty, no text log file is generated. |

Sample:

<pre>
{
    "handlers": {
        "deviceInfo": {
            "reportOnConnect": true,
            "textLogFilesPath": "c:\logs"
        }

    }
}
</pre>

## When Is It Applied?

The start-up configuration will get passed to the handler everytime the `Start()` method is called.
The `Start()` method is typically called on startup and again every time the IoT Hub connection is re-established.

## Conflict Resolution

If a given behavior is controlled by both a property in the start-up configuration and a property in the twin, the handler needs to define the behavior.

Here are some recommendations:

- It is best if a given behavior is only controlled by either the start-up configuration or by the twin.
- If there is an overlap, then the handler should assign a priority. Typically, the priority should be to the twin given its dynamic nature and that if it is being configured, by definition, it is a decision being made after the device has been built and hence takes precendence.

Related Topics

- [Extensibility and Discovery Model](extensibility-and-discovery-model.md)
- [Device Agent Configuration File](reference/device-agent-configuration-file.md)

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)