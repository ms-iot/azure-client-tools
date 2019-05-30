# Enumerate Diagnostic Log Files Command

## Overview

The enumerate log files command lists all the files under the specified folder. The folder should be present in the path defined by `pluginsDataPath` or in the default data path. 

| Handler Identifier | Plug-in Binary |
|----|----|
| `EnumerateLogFilesCmd` | DiagnosticLogsManagementPlugin.dll<br/>cpprest141[d]_2_10.dll<br/>wastorage.dll |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration

| Name | Type | Required | Description |
|------|------|----------|-------------|
| `textLogFilesPath` | string | no | If not empty, the handler will log to a text file created at the path specified. The format of log file name is: AzureDM.deviceInfo.&lt;date_time&gt;.log |
| `pluginsDataPath` | string | no | If empty, the handler will list the files from the folder in the default path `<processpath>\\..\\DMData)`. If not empty, the handler will list the created diagnostic log files from the specified path. |  

Example:

In the [device-agent configuration file](../../reference/device-agent-configuration-file.md), add the following snippet :

<pre>
{
  "textLogFilesPath": "c:\logs",
  "pluginsDataPath": "c:\data"
}
</pre>

## Payload

| Name | Required | Type | Description |
|-----|-----|-----|-----|
| `folderName` | Yes | String | Diagnostic log folder name. |

Sample payload:

<pre>
{
    "__meta": {
        "serviceInterfaceVersion": "1.0.0"
    },
    "folderName": "collectorFolderName"
}
</pre>

## Outputs

- Success:
    - Code: 200
    - Payload:
    <pre>
    {
        "list": [
            "AzureDM_2017_07_18_11_14_38.etl",
            ...,
            "AzureDM_2017_07_20_18_14_38.etl"
        ]
    }
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