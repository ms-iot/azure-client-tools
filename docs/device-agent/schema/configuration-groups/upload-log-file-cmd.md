# Upload Log File to Azure Storage Command

## Overview

The Upload logs command enables the operator to upload a log file to the Azure Storage container.

| Handler Identifier | Plug-in Binary |
|----|----|
| `UploadLogFileCmd` | DiagnosticLogsManagementPlugin.dll<br/>cpprest141[d]_2_10.dll<br/>wastorage.dll |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration

| Name | Type | Required | Description |
|------|------|----------|-------------|
| `textLogFilesPath` | string | no | If not empty, the handler will log to a text file created at the path specified. The format of log file name is: AzureDM.deviceInfo.&lt;date_time&gt;.log |
| `pluginsDataPath` | string | no | If not empty, the handler will save the created diagnostic log files to the specified path. |

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
| `connectionString` | Yes | String | Azure storage connection strings to upload diagnostic log. |
| `container` | Yes | String | Azure storage container name you want to upload the diagnostic logs to. |

Sample:

<pre>
{
    "folderName": "collectorFolderName",
    "fileName": "logFileName",
    "connectionString": "<AzureStorageConnectionString>",
    "container": "AzureStorageContainerName"
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