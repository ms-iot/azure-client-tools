# Incremental Updates and Merging

## Overview

IoT Hub sends a notification to the device with only the leaf nodes that have changed and their parents. For example, assume the current twin state is:

<pre>
{
    "properties":{
        "desired":{
            "group_id": {
                "A": 5,
                "B": 6
            }
        }
    }
}
</pre>

If the operator updates "A", only the following snippet will be sent to the device:

<pre>
{
    "properties":{
        "desired":{
            "group_id": {
                "A": 5
            }
        }
    }
}
</pre>

This design provides a very efficient way of using the network bandwidth.

For sub-groups that consist of single property, this behavior has no impact.

However, for sub-groups that have multiple properties, this requires a way to re-construct the sub-group desired state when a delta is received.

In the example above, the handler should have received the entire sub-group when it was first configured. This means that at some point, the handler learned that the value of "B" is 6.

The handler can then use this knowledge so that when it receives the update snippet ("A": 6), it is able to re-construct that sub-group first before applying it. 

## Merging

"Merging" is when a handler caches the desired state so that when delta updates are received, they can be *merged* into the cached version and the sub-group entire state is present and be presented to the part of the code that parses and applies it.

The `JsonHelpers::Merge()` function provides a utility that can do the merge at the json level.

Merging should take place before parsing and validation - because when parsing starts, the handler should throw errors if fields are missing.

Given that a sub-group is the smallest unit of operation, merging should occur across sub-groups. Otherwise, the handler will attempt to set the other sub-groups again even though the last delta did not include them.

ToDo: we don't have an easy way to do this merge where only changed sub-groups are affected.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)