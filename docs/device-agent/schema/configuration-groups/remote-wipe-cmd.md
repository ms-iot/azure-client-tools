# Remote Wipe Command

## Overview

The Remote wipe command enables the operator to wipe out all data on the device and restore the device to a clean state. Remote wipe does not uninstall any applications or software that got installed.

| Handler Identifier | Plug-in Binary |
|----|----|
| `StartRemoteWipeCmd` | RemoteWipePlugin.dll |

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

## Payload

| Name | Required | Type | Description |
|-----|-----|-----|-----|
| `clearTpm` | No | Bool | If true, it clears the TPM. |

Sample payload:

<pre>
{
    "__meta": {
        "serviceInterfaceVersion": "1.0.0"
    },
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
            "StartRemoteWipeCmd": {
                "__meta": {
                    &lt;meta data object content&gt;
                },
                "__errors": {
                    "StartRemoteWipeCmd": {
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