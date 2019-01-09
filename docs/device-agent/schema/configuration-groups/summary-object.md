# Summary Object

## Overview

The summary object is a twin reported json object that summarizes the state of the device. Such summaries are very efficient in finding devices based on summarized properties.
For more details, see the [Twin Aggregation](../../twin-aggregation.md) page.

## Schema

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- deployments -->
    <tr valign="top">
        <td rowspan="4"><b>Deployment List</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates what deployments affect this device.</td>
        <td rowspan="4">`deployments`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>Hold a list of deployment ids separated by semicolons.</td>
    </tr>
    <!-- failedCount -->
    <tr valign="top">
        <td rowspan="4"><b>Failed Count</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates the number of failed configuration sub-groups.</td>
        <td rowspan="4">`failedCount`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>The number of failed configuration sub-groups.</td>
    </tr>
    <!-- failedGroups -->
    <tr valign="top">
        <td rowspan="4"><b>Failed Groups</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates a list of configuration group identifiers with failures.</td>
        <td rowspan="4">`failedGroups`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>A semicolon separate list of configuration group identifiers where errors are reported.</td>
    </tr>
    <!-- pendingCount -->
    <tr valign="top">
        <td rowspan="4"><b>Pending Count</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates the number of pending configuration sub-groups.</td>
        <td rowspan="4">`pendingCount`</td>
        <td>Type</td><td>integer</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>The number of pending configuration sub-groups.</td>
    </tr>
    <!-- pendingGroups -->
    <tr valign="top">
        <td rowspan="4"><b>Pending Groups</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates a list of configuration group identifiers with failures.</td>
        <td rowspan="4">`pendingGroups`</td>
        <td>Type</td><td>string</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>A semicolon separate list of configuration group identifiers where errors are reported.</td>
    </tr>
    <!-- time -->
    <tr valign="top">
        <td rowspan="4"><b>Time Stamp</b><br/><br/>
            Desired: n/a<br/>
            Reported: Yes<br/><br/>
            Indicates the time stamp of when the summary object was last updated.</td>
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
        <td>Description</td><td>Time in ISO 8601 format (i.e. 2016-10-10T17:00:00Z).</td>
    </tr>
</table>

### Sample

<pre>
  "__summary": {
    "deployments": "",
    "failedCount": 1,
    "failedGroups": "deviceInfo",
    "pendingCount": 0,
    "pendingGroups": "",
    "time": "2018-11-21T11:16:30Z"
  }
</pre>

## Code

This object is represetned in the code with the `ReportedSummary` type. An instance of this type is created and owned by the handlers host (`AzureDMClient`).
When a handler changes its deployment state or reports, the device agent intercepts those calls and updates the summary object, and then aggregates its json representation with the json coming from the handler and reports them as one chunk.
This is important to maintain the integrity of the data in the twin.

----

[Home](../../../../README.md) | [Device Agent](../../device-agent.md) | [Reference](../../reference.md)