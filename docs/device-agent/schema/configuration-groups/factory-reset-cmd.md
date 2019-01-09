# Factory Reset

## Overview

Factory reset re-applies a factory-image on the device restoring all components and their data to the original state. This means that any updates that got installed will not be present after the reset.

| Handler Identifier | Plug-in Binary
|----|----|
| factoryResetCmd | FactoryResetPlugin.dll |

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

## Parameters

| Name | Required | Type | Description |
|-----|-----|-----|-----|
| `partitionGuid` | Yes | String | Guid of the partition holding the recovery image. |
| `clearTpm` | No | Bool | If true, it clears the TPM. |

Sample:

<pre>
{
    "partitionGuid": "guid",
    "clearTpm": true
}
</pre>

## Outputs

### Return Values

- Success:
    - Code: 200
    - Payload:
    <pre>{}</pre>

- Failure:
    - Code: 400
    - Payload:
        <pre>
        {
            &lt;error object content&gt;
        }
        </pre>

### Device Twin

<pre>
{
    "properties": {
        "reported": {
            "factoryResetCmd": {
                "__meta": {
                    &lt;meta data object content&gt;
                },
                "__errors": {
                    "factoryResetCmd": {
                        &lt;error object content&gt;
                    }
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