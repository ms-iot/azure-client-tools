# __deviceSchemas

## Overview

The `__deviceSchemas` node describes what versions of each configuration group the client implements. It is also used to configure how the client should report such information.

Note that if the handler is marked inactive, its schema description will not be reported. This allows the solution developer to determine which capabilities are present and which are not.

| Handler Identifier | Plug-in Binary |
|----|----|
| `__deviceSchemas` | build-in |

## Life Cycle and Start-up Configuration

- Active
    - Always.

- Start-up Configuration

| Name | Type | Required | Description |
|------|------|----------|-------------|
| `handlers.__deviceSchemas.reportOnConnect` | boolean | No | If true, it causes the handler to report its properties once the agent connects to IoT Hub. <br/><br/>This property does not honor the twin `properties.desired.__deviceSchemas.__meta.reportingMode` value because it is processed before the desired configuration is received. <br/><br/>See [Patterns](../patterns.md) for more details. | 

Example:

In the [agent configuration file](../../service-configuration-file.md), add the following snippet under `handlers`:

<pre>
{
  "handlers": {
    "__deviceSchemas": {
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
    <!-- reporting.groups -->
    <tr valign="top">
        <td rowspan="12"><b>Reporting Configuration</b><br/><br/>
            Desired: Required<br/>
            Reported: No<br/><br/>
            Indicates what and how schemas should be reported.</td>
        <td rowspan="4">`reporting.groups`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Values:<br/><br/>
            - "all"<br/>
            - "configured";"&lt;group-id&gt;";"&lt;group-id&gt;"</td>
    </tr>
    <!-- reporting.types -->
    <tr valign="top">
        <td rowspan="4">`reporting.types`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Values:<br/><br/>
            - "all"<br/>
            - "raw"</td>
    </tr>
    <!-- reporting.tags -->
    <tr valign="top">
        <td rowspan="4">`reporting.tags`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Values:<br/><br/>
            "&lt;tag2&gt;";"&lt;tag1&gt;"</td>
    </tr>
    <!-- device schemas list -->
    <tr valign="top">
        <td rowspan="4"><b>Device Schemas List</b><br/><br/>
            Desired: n/a<br/>
            Reported: Optional<br/><br/>
            A list of the schema information for all enumerated handlers.</td>
        <td rowspan="4">`&lt;groupd-id&gt;`</td>
        <td>Type</td><td>&lt;device schema object&gt;</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>ToDo:</td>
    </tr>

</table>

### Sample

<pre>
{
  "properties": {
    "desired": {
      "__deviceSchemas": {
        "__meta": {
            &lt;meta data object content&gt;
        }
        "reporting": {
          "groups": "all|configured;&lt;group1&gt;;&lt;group2&gt;",
          "types": "all|raw",
          "tags": "&lt;tag1&gt;;&lt;tag2&gt;"
        }
    }
  },
  "reported": {
    "__deviceSchemas": {
      "__meta": {
          &lt;meta data object content&gt;
      },
      "__errors": {
          "sub-group-id": {
              &lt;error object content&gt;
          }
          ...
      },
      "&lt;group-id-00&gt;": {
        &lt;device schema object&gt;
      },
      "&lt;group-id-01&gt;": {
        &lt;device schema object&gt;
      },
      ...
    }
  }
}
</pre>

For details on referenced object schemas:

- See the [Meta Data Object](meta-object.md) page.
- See the [Error Object](error-object.md) page.
- See the [Device Schema Object](device-schema-object.md) page.

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md)