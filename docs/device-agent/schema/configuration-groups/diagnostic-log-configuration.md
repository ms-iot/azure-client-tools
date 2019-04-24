# Diagnostic Logs Configuration

## Overview
Event Tracing for Windows (ETW) is a mechanism provided on the Windows platform that allows processes to log messages with minimal overhead.

For trace collection, ETW implements: 

- Provider : the process that is writing the log message
- Collector : the process that is reading the log messages

The relationship between collectors and providers are many-to-many.

Both providers and collectors are identified on the system using Ids (assigned at creation time). The user can then specify which provider a given collector can listen to using those Ids.

The logical hierarchy is as follows:

<pre>
- Collector1
  - LogFileSizeLimitMB = 4
  - LogFileFolder = "AzureDM"
  - Provider1
    - TraceLevel = critical
  - Provider2
    - TraceLevel = error
- Collector2
  - ...
</pre>

Above, Collector1 is defined to listen to two providers: Provider1 and Provider2. Collector1 will write only critical messages from Provider1 and error (or higher priority) messages from Provider2.

### User Scenario
A typical usage scenario is that there is a problem with a certain process running on the system.

If that process does not log using ETW, then, this mechanism is not for it.

If that process is using ETW, then it is a provider and it logs its message to the OS under a certain GUID. The user needs to find out what this GUID is.

Once we have the provider(s) GUID(s), we can define a collector and list the providers underneath - along with all the necessary configurations for the collector and the providers.

Windows IoT Azure Device Agent exposes ETW configuration by wiring-up the [Diagnostic Log CSP](https://docs.microsoft.com/en-us/windows/client-management/mdm/diagnosticlog-csp). The logic and properties described below is very closely based on how the CSP is designed.

Here are the steps to capture logs to a file on disk:

- Identify the provider(s) you want to capture (find out the GUIDs you need to collect).
- Create a collector, configure it, and add the providers you want captured to it.
- Start collection.
- Stop collection. This saves a log file on disk in a pre-configured folder.

Here are the steps to upload a log file:

- Enumerate all the files in the specified log folders.
- Provide the source file name on disk, the target Azure Storage parameters (connection string, container), and the Azure Device Agent client will upload it for you.

| Handler Identifier | Plug-in Binary |
|----|----|
| `eventTracingCollectors` | DiagnosticLogsManagementPlugin.dll<br/>cpprest141[d]_2_10.dll<br/>wastorage.dll |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration

| Name | Type | Required | Description |
|------|------|----------|-------------|
| `textLogFilesPath` | string | no | If not empty, the handler will log to a text file created at the path specified. The format of log file name is: AzureDM.deviceInfo.&lt;date_time&gt;.log |
| `pluginsDataPath` | string | no | If empty, the handler will save the diagnostic log in default path `<processpath>\\..\\DMData)`. If not empty, the handler will save them in the path specified here. |  

Example:

In the [device-agent configuration file](../../reference/device-agent-configuration-file.md), add the following snippet :

<pre>
{
  "textLogFilesPath": "c:\logs",
  "pluginsDataPath": "c:\data"
}
</pre>

## Sub-Groups

Collectors are created by simply defining them in the device twin desired properties section. Each collector exposes a set of properties that defines its operation along with a set of providers and how each of them is processed by the collector.

Below is what the schema looks like:

### Sub-Group Schema

Collector name is defined here and forms the collectors identity in the system. 

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="4"><b>&lt;Collector Sub-Group&gt;</b><br/><br/>
            Desired: Optional<br/>
            Reported: Yes<br/>
            </td>
        <td rowspan="4">`&lt;Collector Name&gt;`</td>
        <td>Type</td><td>Object<br/>Collector Map. See schema below.</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Optional</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Optional</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`&lt;Collector Name&gt;` value is a json object holding a map of collector properties and _meta object. <br/><br/>For the 'applyProperties' context, see the "Collector Configuration Map" section.<br/><br/>For the '_meta' context, see the "Sub-Group Meta Data Object" section.</td>
    </tr>
</table>

### Collector Configuration Map

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="24"><b>Collector Config Map</b><br/><br/>
            Desired: Required<br/>
            Reported: Optional<br/><br/>
         </td>
         <!-- collector config - traceLogFileMode -->
        <td rowspan="4">`traceLogFileMode`</td>
        <td>Type</td><td>String</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Optional</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td>
        <td>`traceLogFileMode` specifies the diagnostic log file logging mode.<br/><br/>
        Values:<br/>
        `sequential`: Logging stops when the file reaches its maximum size.<br/>
        `circular`: After the file reaches the maximum size, the oldest events are replaced with incoming events.<br/>
        </td>
    </tr>
    <!-- collector config - logFileSizeLimitMB -->
    <tr valign="top">
        <td rowspan="4">`logFileSizeLimitMB`</td>
        <td>Type</td><td>Integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Optional</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`logFileSizeLimitMB` specifies the limit for the log file in megabytes.</td>
    </tr>
     <!-- collector config - logFileFolder -->
    <tr valign="top">
        <td rowspan="4">`logFileFolder`</td>
        <td>Type</td><td>String</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`logFileFolder` specifies the relative path to the user's data folder where the log files of that collector will be saved once collection stops. The folder name cannot include \ (The files can later be enumerated and uploaded to Azure Storage. See below for more details).</td>
    </tr>
    <!-- collector config - logFileName -->
    <tr valign="top">
        <td rowspan="4">`logFileName`</td>
        <td>Type</td><td>String</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`logFileName` specifies the name of the log file to be generated. If the value of this property is an empty string, a default one will be generated (collector_name_time_stamp).</td>
    </tr>
    <!-- collector config - started -->
    <tr valign="top">
        <td rowspan="4">`started`</td>
        <td>Type</td><td>Boolean</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`started` specifies whether the collector should be active (i.e. collecting) or not. Its value is applied everytime the DM client service starts, or the property changes.<br/><br/>
        Values:<br/>
        `true`: the collector will be started (if it is not already).<br/>
        `false`: the collector will be stopped, and a file will be saved in logFileFolder (if it is already running).</td>
    </tr>
    <!-- collector config - providers -->
    <tr valign="top">
        <td rowspan="4">`providers`</td>
        <td>Type</td><td>Object</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`providers` value is a json object holding a map of providers and their configuration properties.</td>
    </tr>
      <tr valign="top">
        <td rowspan="4"><b>Meta Data</b><br/><br/>
            Desired: Optional<br/>
            Reported: n/a<br/><br/>
            </td>
        <td rowspan="4">`__meta`</td>
        <td>Type</td><td>Object</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Each collector has a Meta Data Object.</td>
    </tr>
</table>

### Provider Configuration Map

The provider configuration map describes the providers that need to be configured for each collector to listen to.
The configuration of a given provider is identified by its GUID in the map.

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="4"><b>Provider Config Map</b><br/><br/>
            Desired: Required<br/>
            Reported: Yes<br/><br/>
            </td>
        <td rowspan="4">`&lt;Provider GUID&gt;`</td>
        <td>Type</td><td>object<br/>Provider Configuration. See schema below.</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Optional</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`&lt;Provider GUID&gt;` value is a json object holding the provider configuration.<br/><br/>provider desired state is described below.</td>
    </tr>
</table>

### Provider Configuration

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="12"><b>Provider Configuration</b><br/><br/>
            Desired: Required<br/>
            Reported: Optional<br/><br/>
            </td>
            <!-- provider config - traceLevel -->
        <td rowspan="4">`traceLevel`</td>
        <td>Type</td><td>String</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td>
        <td>`traceLevel` specifies the level of detail included in the trace log. <br/><br/>
        Values:<br/> 
        `critical`<br/>
        `error`<br/>
        `warning`<br/>
        `information` <br/>
        `verbose`<br/>
        </td>
    </tr>
    <!-- collector config - keywords -->
    <tr valign="top">
        <td rowspan="4">`keywords`</td>
        <td>Type</td><td>String</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Optional</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`keywords` specifies the provider keywords to be used as MatchAnyKeyword for this provider.</td>
    </tr>
     <!-- collector config - enabled -->
    <tr valign="top">
        <td rowspan="4">`enabled`</td>
        <td>Type</td><td>Boolean</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`enabled` specifies if this provider is enabled in the trace session. </td>
    </tr>
</table>

### Diagnostic Logs Reported Map

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="4"><b>Collector-Config Map</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            </td>
        <td rowspan="4">`&lt;Collector Name&gt;`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`&lt;collector name&gt;` value is always an empty string ("") if the reporting mode is set to `minimal` in the meta object for that collector. This is just a place holder since the twin cannot hold arrays. The value is a json object holding collector properties if the reporting mode is set to `detailed` in the meta object for the collector.</td>
    </tr>
</table>

### Sample

<pre>
{
  "properties": {
    "desired": {
        "eventTracingCollectors": {
            "collector00": {
                  "traceLogFileMode": "sequential",
                  "logFileSizeLimitMB": 500,
                  "logFileFolder": "collectorFolderName",
                  "logFileName": "logFileName",
                  "started": false,
                  "providers": {
                      "guid00": {
                          "traceLevel": "verbose",
                          "keywords": "",
                          "enabled": true
                      }
                    }
                  "__meta": {
                    "reportingMode": "detailed"
                  }
            },
            "collector01": {
                "__meta": {
                    "reportingMode": "minimal"
                }
            }
        }
      },
    "reported": {
      "eventTracingCollectors" :
        {
            "__errors": {
                "sub-group-id": {
                &lt;error object content&gt;
                }
                ...
            },
            "__meta" :
            {
                &lt;meta data object content&gt;
            },
            "collector00" :{
                    "logFileFolder" : "collectorFolderName",
                    "logFileName": "logFileName",
                    "logFileSizeLimitMB" : 500,
                    "providers" :
                    {
                        "guid00" :
                        {
                            "enabled" : true,
                            "keywords" : "0",
                            "traceLevel" : "verbose"
                        }
                    },
                    "started" : true,
                    "traceLogFileMode" : "sequential"
                }
            },
            "collector01": ""
    }
  }
}
</pre>

For details on referenced object schemas:

- See the [Meta Data Object](meta-object.md) page.
- See the [Error Object](error-object.md) page.

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md) | [Certificate Management](certificate-management.md)