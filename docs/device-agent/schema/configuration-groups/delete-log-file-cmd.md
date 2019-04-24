# Delete Log Files Command

## Overview

The delete log files command deletes the specified diagnostic log file from the device storage. The folder/file should be present in the path defined by `pluginsDataPath` or in the default data path. 

| Handler Identifier | Plug-in Binary |
|----|----|
| `DeleteLogFilesCmd` | DiagnosticLogsManagementPlugin.dll<br/>cpprest141[d]_2_10.dll<br/>wastorage.dll |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration

| Name | Type | Required | Description |
|------|------|----------|-------------|
| `textLogFilesPath` | string | no | If not empty, the handler will log to a text file created at the path specified. The format of log file name is: AzureDM.deviceInfo.&lt;date_time&gt;.log |
| `pluginsDataPath` | string | no | If empty, the handler will delete the file from default path `<processpath>\\..\\DMData)`. If not empty, the handler will delete the file from the path specified here. |  

Example:

In the [device-agent configuration file](../../reference/device-agent-configuration-file.md), add the following snippet :

<pre>
{
  "textLogFilesPath": "c:\logs",
  "pluginsDataPath": "c:\data"
}
</pre>

## Parameters

| Name | Required | Type | Description |
|-----|-----|-----|-----|
| `folderName` | Yes | String | Diagnostic log folder name. |
| `fileName` | Yes | String | Diagnostic log file name. |

Sample:

<pre>
{
    "folderName": "collectorFolderName",
    "fileName": "logFileName"
}
</pre>

## Outputs

- Success:
    - Code: 200
    - Payload:
    <pre>
    {}
    </pre>

- Failure:
    - Code: 400
    - Payload:
        <pre>
        {
            &lt;error object content&gt;
        }
        </pre>

### Device Twin

None.

For details on referenced object schemas:

- See the [Meta Data Object](meta-object.md) page.
- See the [Error Object](error-object.md) page.

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md) | [Certificate Management](certificate-management.md)