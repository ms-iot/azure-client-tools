# Certificate Details Get Command

## Overview

The Certificate details get command enables the operator to retrieve details about an installed certificate.

| Handler Identifier | Plug-in Binary |
|----|----|
| `getDetailedCertificateInfo` | CertificateManagementPlugin.dll<br/>cpprest141[d]_2_10.dll<br/>wastorage.dll |

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
  "textLogFilesPath": "c:\logs",
}
</pre>

## Parameters

| Name | Required | Type | Description |
|-----|-----|-----|-----|
| `path` | Yes | String | Certificate store CSP path. See below for a list of supported paths. |
| `hash` | Yes | String | Thumbprint of the installed certificate. |

Sample:

<pre>
{
    "path": "./Vendor/MSFT/CertificateStore/CA/System",
    "hash": "109F1CAED645BB78B3EA2B94C0697C740733031C"
}
</pre>

## Outputs

- Success:
    - Code: 200
    - Payload:
    <pre>
    {
      "base64Encoding" : "<base64 encoding of the certificate content>",
      "issuedBy" : "Microsoft Root Authority",
      "issuedTo" : "Microsoft Windows Hardware Compatibility",
      "templateName" : "",
      "validFrom" : "1997-10-01T07:00:00Z",
      "validTo" : "2002-12-31T07:00:00Z"
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
