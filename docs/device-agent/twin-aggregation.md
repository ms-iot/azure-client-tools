# Twin Aggregation

## Overview

Typically, an operator oversees a large number of devices - in some cases, thousands of devices.
Having an efficient way to locate devices in certain states is critical to the productivity of the operator as well as for cost savings of cloud resources.

If the target state is scattered in the twin reporting section, the operator or the solution developer will have to write very inefficient queries:
<pre>
    select deviceId
    from devices
    where reported.x.result = `failed`
       or reported.y.result = `failed`
       or reported.z.result = `failed`
       or ...
</pre>

To solve this problem, the device agent can aggregate a number of states from the various configuration groups into a single property.

For example, the error state can be aggregated into a single property indicating the number of errors reported by all configuration groups. This allows writing a query like this:

<pre>
    select deviceId
    from devices
    where reported.summary.errorCount <> 0
</pre>

For more details on all the aggregated states, see the [Summary Object](schema/configuration-groups/summary-object.md) page.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)
