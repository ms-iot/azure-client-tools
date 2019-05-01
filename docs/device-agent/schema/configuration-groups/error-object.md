# Error Object

## Overview

The Error Object describes a signle error instance. It is always attached to a node describing the sub-group that is being applied.

Given that the Device Agent utilizes plug-ins that can be authored by various parties, and those plug-ins are expected to utilize various platform components, there is no way to have a predetermined set of errors.

To make the errors as verbose as possible, each error includes fields that help categorize the error and scope it to a certain subsystem, context, and finally an error code that should be unique to this triplet.
See more details about these different pieces in the Schema section below.

Because the IoT Hub twins cannot hold unicode characters, all error messages are ANSI and are not localizable.
Their primary purpose is to make debugging easier. For known ones, the web-application may have translations and display them to the user (making use of the parameters).

## Schema

| Property | Required | Type | Description |
|----|----|----|----|
| `subsystem` | Yes | string | A string identifying whether the error originated from the device management stack or from some lower level stack.<br/><br/>Values:<br/><br/>`"dm"`: error originated from the device management stack.
| `context` | Yes | string | A string identifying what is taking place at the point of failure. |
| `code` | Yes | integer | The error return code from the failed function or sub-system. |
| `message` | Yes | string | A non-localized description of the error. |
| `parameters` | Yes | json object | A json object holding parameters to the error message. For example, if the error message is 'file not found', the parameters can hold the file name. |

### Sample

<pre>
{
    "subsystem": ""
    "context": "StartRebootCmd",
    "code": -2147024891,
    "message": "RegisterDeviceWithLocalManagement",
    "parameters": {},
}
</pre>

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md)