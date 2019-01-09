# Windows Update Management

## Overview

The Windows update management enables the operator to configure the policies that govern the Windows update service on the device.

| Handler Identifier | Plug-in Binary |
|----|----|
| `windowsUpdate` | WindowsUpdatePlugin.dll |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration

| Name | Type | Required | Description |
|------|------|----------|-------------|
| `textLogFilesPath` | string | no | If not empty, the handler will log to a text file created at the path specified. The format of log file name is: AzureDM.deviceInfo.&lt;date_time&gt;.log |

Example:

In the [agent configuration file](../../service-configuration-file.md), add the following snippet under `handlers`:

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
    <!-- activeHoursStart -->
    <tr valign="top">
        <td rowspan="4"><b>Active Hours Start</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Sets the start of active hours.</td>
        <td rowspan="4">`activeHoursStart`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- activeHoursEnd -->
    <tr valign="top">
        <td rowspan="4"><b>Active Hours End</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Sets the end of active hours.</td>
        <td rowspan="4">`activeHoursEnd`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- allowAutoUpdate -->
    <tr valign="top">
        <td rowspan="4"><b>Allow Auto Update</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Enables auto updates.</td>
        <td rowspan="4">`allowAutoUpdate`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- allowUpdateService -->
    <tr valign="top">
        <td rowspan="4"><b>Enable Update Service</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Enables the update service.</td>
        <td rowspan="4">`allowUpdateService`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- branchReadinessLevel -->
    <tr valign="top">
        <td rowspan="4"><b>Branch Readiness Level</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Sets the branch readiness level.</td>
        <td rowspan="4">`branchReadinessLevel`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- deferFeatureUpdatesPeriod -->
    <tr valign="top">
        <td rowspan="4"><b>Defer Feature Update Period</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Defers feature update period.</td>
        <td rowspan="4">`deferFeatureUpdatesPeriod`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- deferQualityUpdatesPeriod -->
    <tr valign="top">
        <td rowspan="4"><b>Defer Quality Updates Period</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Defers quality updates period.</td>
        <td rowspan="4">`deferQualityUpdatesPeriod`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- pauseFeatureUpdates -->
    <tr valign="top">
        <td rowspan="4"><b>Pause Feature Updates</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Pauses feature updates.</td>
        <td rowspan="4">`pauseFeatureUpdates`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- pauseQualityUpdates -->
    <tr valign="top">
        <td rowspan="4"><b>Pause Quality Updates</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Pauses quality updates.</td>
        <td rowspan="4">`pauseQualityUpdates`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- scheduledInstallDay -->
    <tr valign="top">
        <td rowspan="4"><b>Scheduled Install Day</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Sets the scheduled the install day.</td>
        <td rowspan="4">`scheduledInstallDay`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
    <!-- scheduledInstallTime -->
    <tr valign="top">
        <td rowspan="4"><b>Scheduled Install Time</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Sets the scheduled install time.</td>
        <td rowspan="4">`scheduledInstallTime`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>
</table>

### Sample

<pre>
{
  "properties": {
    "desired": {
      "windowsUpdate": {
        "__meta": {
            &lt;meta data object content&gt;
        },
        "activeHoursStart": 1,
        "activeHoursEnd": 2,
        "allowAutoUpdate": 1,
        "allowUpdateService": 0,
        "branchReadinessLevel": 16,
        "deferFeatureUpdatesPeriod": 30,
        "deferQualityUpdatesPeriod": 15,
        "pauseFeatureUpdates": 0,
        "pauseQualityUpdates": 0,
        "scheduledInstallDay": 3,
        "scheduledInstallTime": 2
      }
    },
    "reported": {
      "windowsUpdate": {
        "__meta": {
            &lt;meta data object content&gt;
        },
        "__errors": {
            "sub-group-id": {
                &lt;error object content&gt;
            }
            ...
        },
        "activeHoursEnd": 2,
        "activeHoursStart": 1,
        "allowAutoUpdate": 1,
        "allowUpdateService": 0,
        "branchReadinessLevel": 16,
        "deferFeatureUpdatesPeriodInDays": 0,
        "deferQualityUpdatesPeriodInDays": 0,
        "pauseFeatureUpdates": 0,
        "pauseQualityUpdates": 0,
        "scheduledInstallDay": 3,
        "scheduledInstallTime": 2
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