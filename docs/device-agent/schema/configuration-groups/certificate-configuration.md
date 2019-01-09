# Certificate Configuration

## Overview

The **Certificate Management** functionality allows the operator to perform the following tasks:

- Install a certficate (from a blob storage).
- Uninstall a certificate.
- List installed certificates.

The above tasks can be performed on a pre-defined set of certificate stores through the use of Windows [CSPs](https://docs.microsoft.com/en-us/windows/configuration/provisioning-packages/how-it-pros-can-use-configuration-service-providers). 
Here is a list of available target certificate stores:

- Root CA Trusted Certificates
    - Root
    - CA
    - TrustedPublisher
    - Trusted People
- Certificate Store
    - CA\System
    - Root\System
    - My\User
    - My\System

To installing a certificate, the operator uploads the certificate file to an Azure Storage container first, and then configures the twin with a pointer to that file so that the agent can download it and install it.

| Handler Identifier | Plug-in Binary |
|----|----|
| `certificates` | CertificateManagementPlugin.dll<br/>cpprest141[d]_2_10.dll<br/>wastorage.dll |

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
  "textLogFilesPath": "c:\logs",
}
</pre>

## Sub-Groups

The certificate management handler is implemented using the Windows Certificate [CSPs](https://docs.microsoft.com/en-us/windows/configuration/provisioning-packages/how-it-pros-can-use-configuration-service-providers). This allows the handler to install, list, and uninstall certificates.

Windows Certificate CSPs are identified by a 'path' and there are 8 of such paths that we support in the certificate management handler.

Each path is mapped to a sub-group - where the sub-group id reflects the path it is targeting. Below is the full map of the supported stores.

### Common Properties

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="4"><b>Connection String</b><br/><br/>
            Desired: Optional<br/>
            Reported: Yes<br/><br/>
            </td>
        <td rowspan="4">`connectionString`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>An Azure Storage connection string that can be used to download certificates to install. For uninstall configurations, this property is not needed.</td>
    </tr>
</table>

### CSP Path to Sub-Group Mapping

|    CSP Path |    Certificate Sub-Group               |
|-------------|----------------------------------------|
| ./Device/Vendor/MSFT/RootCATrustedCertificates/Root             | Json Sub-Group Id:<br/>`rootCATrustedCertificates_Root`<br/><br/>Description:<br/>Some Description.|
| ./Device/Vendor/MSFT/RootCATrustedCertificates/CA               | Json Sub-Group Id:<br/>`rootCATrustedCertificates_CA`<br/><br/>Description:<br/>Some Description.|
| ./Device/Vendor/MSFT/RootCATrustedCertificates/TrustedPublisher | Json Sub-Group Id:<br/>`rootCATrustedCertificates_TrustedPublisher`<br/><br/>Description:<br/>Some Description.|
| ./Device/Vendor/MSFT/RootCATrustedCertificates/TrustedPeople    | Json Sub-Group Id:<br/>`rootCATrustedCertificates_TrustedPeople`<br/><br/>Description:<br/>Some Description.|
| ./Vendor/MSFT/CertificateStore/CA/System                        | Json Sub-Group Id:<br/>`certificateStore_CA_System`<br/><br/>Description:<br/>Some Description.|
| ./Vendor/MSFT/CertificateStore/Root/System                      | Json Sub-Group Id:<br/>`certificateStore_Root_System`<br/><br/>Description:<br/>Some Description.|
| ./Vendor/MSFT/CertificateStore/My/User                          | Json Sub-Group Id:<br/>`certificateStore_My_User`<br/><br/>Description:<br/>Some Description.|
| ./Vendor/MSFT/CertificateStore/My/System                        | Json Sub-Group Id:<br/>`certificateStore_My_System`<br/><br/>Description:<br/>Some Description.|

### Sub-Group Schema

For each of the above sub-groups, the schema is:

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="4"><b>&lt;Certificate Sub-Group&gt;</b><br/><br/>
            Desired: Optional<br/>
            Reported: Yes<br/><br/>
            </td>
        <td rowspan="4">`&lt;Json Sub-Group-Id&gt;`</td>
        <td>Type</td><td>object<br/>Store Certificates Map. See schema below.</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`&lt;Certificate Sub-Group-Id&gt;` value is a json object holding a map of certificate thumbprint to certificate desired state or certificate reported state (depending on the context).<br/><br/>For the 'desired' context, see the "Store Certificate Configuration Map" section.<br/><br/>For the 'reported' context, see the "Store Certificate Reported Map" section.</td>
    </tr>
</table>

### Store Certificate Configuration Map

The store certificate map describes what certificates need to be configured for that particular store.
The configuration of a given certificate is identified by its thumbprint in the map.

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="4"><b>Thumbprint-Config Map</b><br/><br/>
            Desired: Required<br/>
            Reported: Yes<br/><br/>
            </td>
        <td rowspan="4">`&lt;Certificate Thumbprint&gt;`</td>
        <td>Type</td><td>object<br/>Certificate Configuration. See schema below.</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`&lt;Certificate Thumbprint&gt;` value is a json object holding the certificate configuration.<br/><br/>Certificate desired state is described below.</td>
    </tr>
</table>

### Store Certificate Configuration

For each certificate, the operator my configure its state to be `installed` or `uninstalled`. If the desired state is `installed`, an Azure Storage connection string, container name, and file name must be present.

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="8"><b>Certificate Configuration</b><br/><br/>
            Desired: Optional<br/>
            Reported: No<br/><br/>
            </td>
        <td rowspan="4">`state`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`state` describes the desired state of the certificate at hand (identified by the thumbprint).<br/><br/>Values:<br/>`installed`<br/>`uninstalled`.<br/><br/>Note that if the state is set to `installed`, the `connectionString` and the `fileName` must be also present.</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="4">`fileName`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`fileName` describes the Azure Storage container and file name from where to download the certificate to install. This field is required if installing a certificate.<br/><br/>The format of the value should be:<br/>"containerName\\fileName".</td>
    </tr>
</table>

### Store Certificate Reported Map

The store certificate map describes what certificates are present for that particular store.
The state of a given certificate is identified by its thumbprint in the map.

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group -->
    <tr valign="top">
        <td rowspan="4"><b>Thumbprint-Config Map</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            </td>
        <td rowspan="4">`&lt;Certificate Thumbprint&gt;`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`&lt;Certificate Thumbprint&gt;` value is always an empty string (""). This is just a place holder since the twin cannot hold arrays.</td>
    </tr>
</table>

### Sample

<pre>
{
  "properties": {
    "desired": {
      "certificates": {
        "__meta": {
            &lt;meta data object content&gt;
        },
        "connectionString": "&lt;Azure Storage connection string&gt;",
        "rootCATrustedCertificates_Root": {
          "09DE264388CCF8607966266135DA76E0B8D7798B": {
            "state": "installed",
            "fileName": "certificates\\gmileka0927.cer"
          },
          "09DE264388CCF8607966266135DA76E0B8DAAAAA": {
            "state": "uninstalled"
          }
        },
        "rootCATrustedCertificates_CA": {},
        "rootCATrustedCertificates_TrustedPublisher": {},
        "rootCATrustedCertificates_TrustedPeople": {},
        "certificateStore_CA_System": {},
        "certificateStore_Root_System": {},
        "certificateStore_My_User": {},
        "certificateStore_My_System": {}
      }
    },
    "reported": {
      "certificates": {
        "__meta": {
            &lt;meta data object content&gt;
        },
        "__errors": {
            "sub-group-id": {
                &lt;error object content&gt;
            }
            ...
        },
        "rootCATrustedCertificates_Root": {
            "09DE264388CCF8607966266135DA76E0B8D7798B": "",
            ...
        },
        "rootCATrustedCertificates_CA": {
            ...
        },
        "rootCATrustedCertificates_TrustedPublisher": {
            ...
        },
        "rootCATrustedCertificates_TrustedPeople": {
            ...
        },
        "certificateStore_CA_System": {
            ...
        },
        "certificateStore_Root_System": {
            ...
        },
        "certificateStore_My_User": {
            ...
        },
        "certificateStore_My_System": {
            ...
        }
      }
    }
  }
}
</pre>

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md) | [Certificate Management](certificate-management.md)
