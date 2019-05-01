# Windows Telemetry Management

## Overview

The Windows Telemetry Management configuration group enables the operator to configure the telemetry level being reported from their Windows machines.

| Handler Identifier | Plug-in Binary |
|----|----|
| `windowsTelemetry` | WindowsTelemetryPlugin.dll |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration

| Name | Type | Required | Description |
|------|------|----------|-------------|
| `textLogFilesPath` | string | no | If not empty, the handler will log to a text file created at the path specified. The format of log file name is: AzureDM.deviceInfo.&lt;date_time&gt;.log |

Example:

In the [device-agent configuration file](../../reference/device-agent-configuration-file.md), add the following snippet under `handlers`:

<pre>
{
  "textLogFilesPath": "c:\logs"
}
</pre>

## Sub-Groups

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- level -->
    <tr valign="top">
        <td rowspan="4"><b>Telemetry Level</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Defines the telemetry level reported by Windows.</td>
        <td rowspan="4">`level`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Levels:<br/>
            - "security"<br/>
            - "basic"<br/>
            - "enhanced"<br/>
            - "full".</td>
    </tr>
</table>

### Sample

<pre>
{
  "properties": {
    "desired": {
      "windowsTelemetry": {
        "__meta": {
            &lt;meta data object content&gt;
        },
        "level": "security|basic|enhanced|full"
      }
    },
    "reported": {
      "windowsTelemetry": {
        "__meta": {
            &lt;meta data object content&gt;
        },
        "__errors": {
            "sub-group-id": {
                &lt;error object content&gt;
            }
            ...
        },
        "level": "security|basic|enhanced|full"
      }
    }
  }
}
</pre>

For details on referenced object schemas:

- See the [Meta Data Object](meta-object.md) page.
- See the [Error Object](error-object.md) page.

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md)