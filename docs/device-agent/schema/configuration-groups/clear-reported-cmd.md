# Clear Reported Properties

## Overview

The clear reported properties command enables the operator to delete all reported properties in the device twin. This can be useful for testing scenarios or for recovering from states where the client is unable to recover from.

| Handler Identifier | Plug-in Binary
|----|----|
| `clearReportedCmd` | built-in |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration
    - None.

## Payload

- None

Sample payload:

<pre>
{}
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
            "clearReportedCmd": {
                "__meta": {
                    &lt;meta data object content&gt;
                },
                "__errors": {
                    "clearReportedCmd": {
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