# Schema Tables

## Overview

The device agent and its capabilities (handlers) communicate with the cloud through a pre-defined json schema.

The high-level description of this schema is as follows:

<pre>
{
    "desired":{
        "configuration-group-id": {
            [sub-group-0-start][n/a|optional|required]
            "sub_group_0_prop_0": "value" [n/a|optional|required],
            "sub_group_0_prop_1": "value" [n/a|optional|required],
            [sub-group-0-end]
            [sub-group-1-start][n/a|optional|required]
            "sub_group_1_prop_0": "value" [n/a|optional|required],
            "sub_group_1_prop_1": "value" [n/a|optional|required]
            [sub-group-1-end]
        }
    },
    "reported":{
        "configuration-group-id": {
            [sub-group-0-start][yes|no]
            "sub_group_0_prop_0": "value" [yes|no],
            "sub_group_0_prop_1": "value" [yes|no],
            [sub-group-0-end]
            [sub-group-1-start][yes|no]
            "sub_group_1_prop_0": "value" [yes|no],
            "sub_group_1_prop_1": "value" [yes|no]
            [sub-group-1-end]
        }
    }
}
</pre>

#### Notes

- Anything between square brackets is an annotation describing the intention and is not an actual attribute in the json.
- Sub-groups are the logical children of a group. Properties are the logical children of a sub-group. For more on groups and sub-groups, see the [Configuration Granularity](configuration-granularity.md) page.

#### Desired Schema

- When a desired property is marked 'required', it is required in relative to its sub-group. However, the sub-group as a whole can be optional.
- When a desired sub-group is marked n/a, consequently all its properties are n/a.

#### Reported Schema
- A sub-group marked to be reported ('yes') does not necessarily mean all its properties will be reported. That's why each individual property can indicate whether it is reported or not.
- On the other hand, if a sub-group is marked to be not reported ('no'), all its properties will be marked 'no' too.

## The Schema Table Format

To represent the above relationships, each handler fills out a table in the following format:

<table>
    <col width="200">
    <tr>
        <td>Sub-Group</td>
        <td colspan="3">Properties</td>
    </tr>
    <!-- sub-group-id -->
    <tr valign="top">
        <td rowspan="8"><b>Sub-Group-0 Title</b><br/><br/>
            Desired: n/a|optiona|required<br/>
            Reported: Yes|No<br/><br/>
            Sub-Group-0 description.</td>
        <td rowspan="4">`sub_group_0_prop_0`</td>
        <td>Type</td><td>data_type</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a|optiona|required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes|No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>sub_group_0_prop_0 description and values.</td>
    </tr>
    <!-- sub-group-id -->
    <tr valign="top">
        <td rowspan="4">`sub_group_0_prop_1`</td>
        <td>Type</td><td>data_type</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a|optiona|required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes|No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>sub_group_0_prop_1 description and values.</td>
    </tr>
    <!-- sub-group-id -->
    <tr valign="top">
        <td rowspan="8"><b>Sub-Group-1 Title</b><br/><br/>
            Desired: n/a|optiona|required<br/>
            Reported: Yes|No<br/><br/>
            Sub-Group-1 description.</td>
        <td rowspan="4">`sub_group_0_prop_0`</td>
        <td>Type</td><td>data_type</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a|optiona|required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes|No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>sub_group_0_prop_0 description and values.</td>
    </tr>
    <!-- sub-group-id -->
    <tr valign="top">
        <td rowspan="4">`sub_group_0_prop_1`</td>
        <td>Type</td><td>data_type</td>
    </tr>
    <tr valign="top">
        <td>Desired</td><td>n/a|optiona|required</td>
    </tr>
    <tr valign="top">
        <td>Reported</td><td>Yes|No</td>
    </tr>
    <tr valign="top">
        <td>Description</td><td>sub_group_0_prop_1 description and values.</td>
    </tr>

</table>

Related Topics:

- [Configuration Granularity](configuration-granularity.md)
- [Correlating Desired and Reported Properties](correlating-desired-and-reported-properties.md)

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)