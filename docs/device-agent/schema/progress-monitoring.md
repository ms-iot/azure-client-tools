# 7. Progress Monitoring

[Schema](../schema.md)

## 7.1. Overall Status

After the operator deploys a new configuration to a set of devices, there needs to be a way to monitor the progress of that deployment – specifically: 

1. Which devices are in the process of applying new configurations... 
2. How many Configuration Groups have been applied.
3. How many Configuration Groups are pending.

re.#2. As new batches of configurations arrive at the device, the number of applied configuration will keep increasing and will lose its meaning. We will not track this number.

A possible schema to address the above requirements is:

<pre>
"reported": {
    "__summary": {
        "pendingCount": &lt;count&gt;,
        "pendingGroups": "apps:appXYZ;appABC|windowsUpdate"
    }
}
</pre>

Figure 7.1.A 

With the schema above, we can write queries like: 

<pre>
SELECT deviceId, reported.__summary.pendingCount FROM Devices  
WHERE reported.__summary.pendingCount!=0
</pre>

Figure 7.1.B

This query retrieves all the devices that are still in progress, and how many groups are left to be processed in each.

## 7.2. Per-Group Status

When a set of configuration groups are updated, the device will get a notification with all those changes. When the device client starts processing the batch, it should mark all sections as being in `pending`.
This allows observers of the device twin to: 

- Know that the state previously reported for that group is about to change and might not reflect the actual state.
- Know which specific group is being processed right now.

The following schema can represent that:

<pre>
"reported": 
    "windowsUpdate": { 
        "__meta": { 
            "time": "time the state was reported",
            "state": "pending"
        } 
    } 
</pre>

Figure 7.2.A

This enables us to write queries as follows: 

<pre>
SELECT * FROM Devices
WHERE reported.windowsUpdate.__meta.state!='pending''
</pre>

Figure 7.2.B

This query filters out those devices which are in the process of being updated.

If the operation completes successfully, we will indicate that by replacing the "pending" state with "succeeded" or "failed" depending on the outcome of the operation (see [Reporting Status](reporting-status.md)).

----

Last Updated 9/6/2018

[Schema](../schema.md)