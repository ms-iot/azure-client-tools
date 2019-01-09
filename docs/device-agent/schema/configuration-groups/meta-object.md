# Meta Data Object

## Overview

We can split the properties in the twin into two categories:

1. Properties that describe the state of a component on the device different from the device agent.
2. Properties that describe how the device agent or one of its handlers should be behave.

When the propeties at hand describe how a handler should behave, such properties are embedded in the configuration group of that handler under the `__meta` node.
Below is a description of node schema and programming model.

## Schema

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- deployment id -->
    <tr valign="top">
        <td rowspan="4"><b>Deployment Id</b><br/><br/>
            Desired: Optional<br/>
            Reported: Yes<br/><br/>
            Indicates the source deployment id of the current configuration.</td>
        <td rowspan="4">`deploymentId`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>This is a string selected by the operator to identify the deployment by.</td>
    </tr>
    <!-- deployment status -->
    <tr valign="top">
        <td rowspan="4"><b>Deployment Status</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates the current status of the deployment.</td>
        <td rowspan="4">`deploymentStatus`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Indicates the status of the underlying configuration. Values are:<br/>
            - notStarted<br/>
            - succeeded<br/>
            - pending<br/>
            - failed
            </td>
    </tr>
    <!-- dependencies -->
    <tr valign="top">
        <td rowspan="4"><b>Dependencies</b><br/><br/>
            Desired: Optional<br/>
            Reported: Yes<br/><br/>
            Lists the group configuration identifiers the meta data group configuration depends on.</td>
        <td rowspan="4">`dependencies`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>A semicolon separate list of group configuration identifiers.</td>
    </tr>
    <!-- reporting mode -->
    <tr valign="top">
        <td rowspan="4"><b>Reporting Mode</b><br/><br/>
            Desired: Optional<br/>
            Reported: Yes<br/><br/>
            Indicates to the agent whether to report the current state of the group configuration or not.</td>
        <td rowspan="4">`dependencies`</td>
        <td>Type</td><td>boolean</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>Required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>`true` indicates the current component status should be reported.<br/>
        `false` indicates the current component status should not be reported to the device twin. If it has been reported in the past, it should be removed.</td>
    </tr>
    <!-- time -->
    <tr valign="top">
        <td rowspan="4"><b>Time Stamp</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates the time of when the reported meta data was last updated.</td>
        <td rowspan="4">`time`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>time in ISO 8601 format (i.e. 2016-10-10T17:00:00Z).</td>
    </tr>
</table>

### Sample

<pre>
{
    "properties" : {
        "desired": {
            "&lt;config-group-id&gt;": {
                "__meta" : {
                    "deploymentId": "unspecified",
                    "dependencies": "x;y",
                    "reportingMode": "never|always",
                }
            }
        },
        "reported": {
            "&lt;config-group-id&gt;": {
                "__meta" : {
                    "deploymentId": "unspecified",
                    "dependencies": "x;y",
                    "reportingMode": "never|always",
                    "deploymentStatus": "notStarted|succeeded|pending|failed",
                    "time": "&lt;time&gt;"
                }
            }
        }
    }
}
</pre>

## Programming Model: The `MetaData` Class

The schema described above is implemented using the `MetaData` class in the common library. The implementation provides an easy way to parse, validate, query, and update the underlying json.

The `BaseHandler` class provide a data member `_metaData` (of type `MetaData`) to hold unto it. The author of the handler has to re-populate it during every invoke and use it to enforce policies (like reporting, etc).

For example, below is a common pattern of using the _metaData:

<pre>
    Invoke(...)
    {
        RunOperation(GetId(), errorList,
            [&]()
        {
            // Make sure this is not a transient state
            if (IsRefreshing(groupDesiredConfigJson))
            {
                return;
            }

            // Processing Meta Data
            _metaData->FromJsonParentObject(groupDesiredConfigJson);

            // Apply new state
            SetSubGroup(...);

            // Report current state
            if (_metaData->GetReportingMode() == JsonReportingModeAlways)
            {
                BuildReported(reportedObject, errorList);
            }
            else
            {
                EmptyReported(reportedObject);
            }
        });

        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }
</pre>

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md)