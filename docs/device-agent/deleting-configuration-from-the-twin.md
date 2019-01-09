# Deleting Configuration From The Twin

## What It Implies

The convention for handlers is that if a component does not have configuration in the twin, it does not mean the component is not present or has been deactivated.
Instead, it means that the operator is not interested in configuring it - or in other words, its current state is acceptable.

This applies for both the desired and the reported sections in the twin.

This can be extended to deleting configurations. If an operator configures a component to be in a certain state, and that configuration is later removed from the twin, the expectation is that the component retains its last state.

For example, if the operator configures three applications on the device, it doesn't mean that there are only three applications on the device. Rather, it means the operator is interested in only those three.

This leads to a question on how to remove something from the device (for example, remove an application) if the absence of its configuration is not the trigger...

This can be expressed with an explicit 'absent' or 'uninstalled' desired state. For example,

<pre>
{
    "properties": {
        "desired": {
            "apps": {
                "app00": {
                    "state": "uninstalled"
                }
            }
        }
    }
}
</pre>

## Implementation

In order to delete a node from the json tree in the twin, its value needs to be set to null. In case of setting a desired property to null, the device client receives a notification of this change.

The implementation need to distinguish between when what is being written is all that should be there vs. it's a delta update.

For example, let's consider a reported application list:

<pre>
{
    "properties": {
        "reported": {
            "apps": {
                "app00": {
                    "state": "installed",
                    "version": "1.0"
                },
                "app01": {
                    "state": "installed",
                    "version": "1.0"
                }
            }
        }
    }
}
</pre>

A robust way of populating the list above is to simply query the list of applications on the device and build a json object and report it to the twin. This approach decouples reporting from desired.

So, the implementation runs the query, and then finds only "app00" installed - so, constructed list is:

<pre>
{
    "properties": {
        "reported": {
            "apps": {
                "app00": {
                    "state": "installed",
                    "version": "1.0"
                }
            }
        }
    }
}
</pre>

When such a list is reported, the twin will still retain "app01" because it did not receive an "app01"=null.

One solution is to explicitly include the "app01"=null.

<pre>
{
    "properties": {
        "reported": {
            "apps": {
                "app00": {
                    "state": "installed",
                    "version": "1.0"
                },
                "app01": null
            }
        }
    }
}
</pre>

That would work. However, writing code that generates that null assignment requires the agent to keep track of what just got removed, and whether it's in the twin already or not. 
It also needs a way to do that that would work across restarts - for example, if after removing the application, the agent got restarted before it gets a chance to remove it from the reported properties. 
In such cases, the handler has to retrieves a copy of the reported properties on start-up and use it as a base to know what needs to be cleaned up.

A simpler approach is to rebuilt the parent's node in the twin by first setting it to null and then following it with the new list.
Given that this approach will compromised the data integrity of the twin if somebody looks at it while the parent is being rebuilt (i.e. nothing is reported), a mitigation is to indicate it is being rebuilt.

So, the steps are:
- Set parent to a string indicating it is being rebuilt (existing plug-ins use "refreshing").
    <pre>
    {
        "properties": {
            "reported": {
                "apps": "refreshing"
            }
        }
    }
    </pre>

- Set the parent to the new json object when it is ready.
    <pre>
    {
        "properties": {
            "reported": {
                "apps": {
                    "app00": {
                        "state": "installed",
                        "version": "1.0"
                    }
                }
            }
        }
    }
    </pre>

This means that whoever is reading the twin should check whether it is "refreshing" - if yes, then can dismiss the state as a transient state and wait to be notified when the value changes again.

Note that this applies for both:

- Device handler that's reading its desired configuration.
- Solution that's reading reported properties.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)