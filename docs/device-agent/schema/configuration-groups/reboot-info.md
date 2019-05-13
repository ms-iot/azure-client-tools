# Reboot Information

## Overview

The Reboot Information configuration group enables the operator to schedule reboots on the device.

| Handler Identifier | Plug-in Binary |
|----|----|
| `rebootInfo` | RebootPlugin.dll |

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
    <!-- singleRebootTime -->
    <tr valign="top">
        <td rowspan="4"><b>Single Time Reboot</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Schedules a one-time reboot.</td>
        <td rowspan="4">`singleRebootTime`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Date and time of the scheduled reboot in ISO 8601 format (i.e. 2016-10-10T17:00:00Z).</td>
    </tr>
    <!-- dailyRebootTime -->
    <tr valign="top">
        <td rowspan="4"><b>Daily Reboot Schedule</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Indicates whether the battery is present or not present and if present, whether it is charging or not.</td>
        <td rowspan="4">`dailyRebootTime`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Date and time of the scheduled reboot in ISO 8601 format (i.e. 2016-10-10T17:00:00Z).</td>
    </tr>
    <!-- lastBootTime -->
    <tr valign="top">
        <td rowspan="4"><b>Last Reboot Time</b><br/><br/>
            Desired: n/a<br/>
            Reported: Optional<br/><br/>
            Last time the device (re)booted.</td>
        <td rowspan="4">`lastBootTime`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Date and time of the scheduled reboot in ISO 8601 format (i.e. 2016-10-10T17:00:00Z).</td>
    </tr>
</table>

### Sample

<pre>
{
  "properties": {
    "desired": {
      "rebootInfo": {
        "__meta": {
          "deploymentId": "reboot_schedule_v1",
          "serviceInterfaceVersion": "1.0.0"
        },
        "singleRebootTime": "2016-11-11T17:00:00Z",
        "dailyRebootTime": "2016-10-10T17:00:00Z"
      }
    },
    "reported": {
      "rebootInfo": {
        "__meta": {
          "deploymentStatus": "succeeded",
          "deploymentId": "reboot_schedule_v1",
          "deviceInterfaceVersion": "1.0.0",
          "time": "..."
        },
        "singleRebootTime": "2016-11-11T17:00:00Z",
        "dailyRebootTime": "2016-10-10T17:00:00Z",
        "lastBootTime": "2018-09-24T19:29:24Z"
      }
    }
  }
}
</pre>

For details on referenced object schemas:

- See the [Meta Data Object](meta-object.md) page.
- See the [Error Object](error-object.md) page.

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md) | [Reboot Management](reboot-management.md)