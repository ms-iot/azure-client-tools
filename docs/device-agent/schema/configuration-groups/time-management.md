# Time Management

## Overview

The Time Management configuration group enables the operator to configure the NTP server as well as the time zone of the device.

| Handler Identifier | Plug-in Binary |
|----|----|
| `timeInfo` | TimePlugin.dll |

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
    <!-- NTP Server -->
    <tr valign="top">
        <td rowspan="4"><b>NTP Server</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Used to synchronize the clock on the device.</td>
        <td rowspan="4">`ntpServer`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>The name of an NTP server (i.e. time.windows.com).</td>
    </tr>
    <!-- Time Zone - dynamicDaylightTimeDisabled -->
    <tr valign="top">
        <td rowspan="44"><b>Time Zone</b><br/><br/>
            Desired: Optional<br/>
            Reported: Optional<br/><br/>
            Used to set the time zone on the device.</td>
        <td rowspan="4">`dynamicDaylightTimeDisabled`</td>
        <td>Type</td><td>boolean</td>
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
    <!-- Time Zone - timeZoneKeyName -->
    <tr valign="top">
        <td rowspan="4">`timeZoneKeyName`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Time zone name.</td>
    </tr>
    <!-- Time Zone - timeZoneBias -->
    <tr valign="top">
        <td rowspan="4">`timeZoneBias`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Bias relative to GMT - in minutes.</td>
    </tr>
    <!-- Time Zone - timeZoneDaylightBias -->
    <tr valign="top">
        <td rowspan="4">`timeZoneDaylightBias`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Daylight savings bias relative to Bias - in minutes.</td>
    </tr>
    <!-- Time Zone - timeZoneDaylightDate -->
    <tr valign="top">
        <td rowspan="4">`timeZoneDaylightDate`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Local time in the format: YYYY-MM-DDThh:mm:ss</td>
    </tr>
    <!-- Time Zone - timeZoneDaylightName -->
    <tr valign="top">
        <td rowspan="4">`timeZoneDaylightName`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Time zone daylight name.</td>
    </tr>
    <!-- Time Zone - timeZoneDaylightDayOfWeek -->
    <tr valign="top">
        <td rowspan="4">`timeZoneDaylightDayOfWeek`</td>
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
    <!-- Time Zone - timeZoneStandardBias -->
    <tr valign="top">
        <td rowspan="4">`timeZoneStandardBias`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Standard savings bias relative to Bias - in minutes.</td>
    </tr>
    <!-- Time Zone - timeZoneStandardDate -->
    <tr valign="top">
        <td rowspan="4">`timeZoneStandardDate`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Local time in the format: YYYY-MM-DDThh:mm:ss</td>
    </tr>
    <!-- Time Zone - timeZoneStandardName -->
    <tr valign="top">
        <td rowspan="4">`timeZoneStandardName`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Time zone standard name.</td>
    </tr>
    <!-- Time Zone - timeZoneStandardDayOfWeek -->
    <tr valign="top">
        <td rowspan="4">`timeZoneStandardDayOfWeek`</td>
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
    "desired" : {
      "timeInfo": {
        "__meta": {
          &lt;meta data object content&gt;
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
    },
    "reported": {
      "timeInfo": {
        "__meta": {
          &lt;meta data object content&gt;
        },
        "__errors": {
          "sub-group-id": {
            &lt;error object content&gt;
          }
          ...
        },
        "ntpServer": "time.windows.com",
        "timeZoneBias": 420,
        "dynamicDaylightTimeDisabled": false,
        "timeZoneKeyName": "Mountain Standard Time",
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
  }
}
</pre>

For details on referenced object schemas:

- See the [Meta Data Object](meta-object.md) page.
- See the [Error Object](error-object.md) page.

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md)