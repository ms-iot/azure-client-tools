# Device Information

## Overview

The device information configuration group describes various aspects of the device hardware and firmware.

| Handler Identifier | Plug-in Binary |
|----|----|
| `deviceInfo` | DeviceInfoPlugin.dll |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration

| Name | Type | Required | Description |
|------|------|----------|-------------|
| `textLogFilesPath` | string | no | If not empty, the handler will log to a text file created at the path specified. The format of log file name is: AzureDM.deviceInfo.&lt;date_time&gt;.log |
| `handlers.deviceInfo.reportOnConnect` | boolean | No | If true, it causes the handler-in to report its properties once the agent connects to IoT Hub. <br/><br/>This property does not honor the twin `properties.desired.deviceInfo.__meta.reportingMode` value because it is processed before the desired configuration is received. <br/><br/>See [Patterns](../patterns.md) for more details. | 

Example:

In the [agent configuration file](../../service-configuration-file.md), add the following snippet under `handlers`:

<pre>
{
  "textLogFilesPath": "c:\logs",
  "handlers": {
    "deviceInfo": {
      "reportOnConnect": true
    }
  }
}
</pre>

## Sub-Groups

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- battery status -->
    <tr valign="top">
        <td rowspan="4"><b>Battery Status</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates whether the battery is present or not present and if present, whether it is charging or not.</td>
        <td rowspan="4">`batteryStatus`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Values:<br/><br/>
            1: High<br/>2: Low<br/>
            4: Critical<br/>
            8: Charging<br/>
            128: No system battery<br/>
            255: Unknown status.<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- battery remaining runtime -->
    <tr valign="top">
        <td rowspan="4"><b>Battery Remaining Runtime</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates how much more time the battery can power the device.</td>
        <td rowspan="4">`batteryRemaining`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>The number of seconds of battery life remaining when the device is not connected to an AC power source. When it is connected to a power source, the value is -1. When the estimation is unknown, the value is -1.<br/><br/>The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- battery maximum runtime -->
    <tr valign="top">
        <td rowspan="4"><b>Battery Maximum Runtime</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates how much time the battery can power the device when fully charged.</td>
        <td rowspan="4">`batteryRuntime`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>The value is the number of seconds of battery life when full charged. When the estimation is unknown, the value is -1.<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- display resolution -->
    <tr valign="top">
        <td rowspan="4"><b>Display Resolution</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates the display width and height resolution in pixels.</td>
        <td rowspan="4">`displayResolution`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>The value is a string in the form "WxH"; for example "1024x768".<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- device management version -->
    <tr valign="top">
        <td rowspan="4"><b>Device Management Version</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`dmVer`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- firmware version -->
    <tr valign="top">
        <td rowspan="4"><b>Firmware Version</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`fwVer`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- hardware version -->
    <tr valign="top">
        <td rowspan="4"><b>Hardware Version</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`hwVer`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- device id -->
    <tr valign="top">
        <td rowspan="4"><b>Device Id</b><br/><br/>
            Indicates ...</td>
        <td rowspan="4">`id`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- language -->
    <tr valign="top">
        <td rowspan="4"><b>Language</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`language`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- manufacturer -->
    <tr valign="top">
        <td rowspan="4"><b>Manufacturer</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`manufacturer`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- model -->
    <tr valign="top">
        <td rowspan="4"><b>Model</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`model`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- name -->
    <tr valign="top">
        <td rowspan="4"><b>Name</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`name`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- oem -->
    <tr valign="top">
        <td rowspan="4"><b>OEM</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`oem`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- osEdition -->
    <tr valign="top">
        <td rowspan="4"><b>OS Edition</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`osEdition`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- osPlatform -->
    <tr valign="top">
        <td rowspan="4"><b>OS Platform</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`osPlatform`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- processorArchitecture -->
    <tr valign="top">
        <td rowspan="4"><b>Processor Architecture</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`processorArchitecture`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- processorType -->
    <tr valign="top">
        <td rowspan="4"><b>Processor Type</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`processorType`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- radioSwVer -->
    <tr valign="top">
        <td rowspan="4"><b>Radio Software Version</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`radioSwVer`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- secureBootState -->
    <tr valign="top">
        <td rowspan="4"><b>Secure Boot State</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`secureBootState`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- swVer -->
    <tr valign="top">
        <td rowspan="4"><b>Software Version</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`swVer`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- totalMemory -->
    <tr valign="top">
        <td rowspan="4"><b>Total Memory</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`totalMemory`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
    <!-- type -->
    <tr valign="top">
        <td rowspan="4"><b>Type</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates ...</td>
        <td rowspan="4">`type`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:<br/><br/>
            The `reporting` desired property controlled whether this property is reported or not.</td>
    </tr>
</table>

### Sample

<pre>
{
  "properties": {
    "desired": {
      "deviceInfo": {
        "__meta": {
            &lt;meta data object content&gt;
        }
      }
    },
    "reported": {
      "deviceInfo": {
        "__meta": {
            &lt;meta data object content&gt;
        },
        "__errors": {
            "sub-group-id": {
                &lt;error object content&gt;
            }
            ...
        },
        "batteryRemaining": -1,
        "batteryRuntime": -1,
        "batteryStatus": 1,
        "displayResolution": "1024x768",
        "dmVer": "1.3",
        "fwVer": "M60 v02.31",
        "hwVer": "M60 v02.31",
        "id": "{41B2B5BA-7DD4-41A8-9DBF-7B491C750E88}",
        "language": "en-US",
        "manufacturer": "Hewlett-Packard",
        "model": "HP Z440 Workstation",
        "name": "GMILEKA9",
        "oem": "Hewlett-Packard",
        "osEdition": "4",
        "osPlatform": "Windows 10 Enterprise",
        "processorArchitecture": "9",
        "processorType": "8664",
        "radioSwVer": "Not Present",
        "secureBootState": 1,
        "swVer": "6.2.9200.1",
        "totalMemory": 32768,
        "type": "HP Z440 Workstation"
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