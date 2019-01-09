# 9. Other Scenarios

[Schema](../schema.md)

## 9.1. Remote vs. Local Policy 

Some of the DM functionality can be controlled by local processes running on the system and cause a conflict in the settings with the desired state as defined in the remote device twin. 

If all changes go through the same channel on the device, and assuming the device is aware of the source of the changes, then a policy can be used to say which one takes precedence. 

Note that such policy should be dynamic. For example, the default setting for Windows Update is to allow the device user (i.e. let’s say an employee) to control it. The operator is okay with that until there is a major security issue and all devices must be ready to download and install the security fix. In that scenario, we need a way for the remote operator to override the user setting on the device. 

The following schema can be used for to express that: 

<pre>
{
    "reported": {
        "windowsUpdate": { 
            "__meta": { 
                ... 
                "policy": "local|remote" 
            }
        }
    }
}
</pre>

Figure 9.1.A

The client implementation will check the "policy" current value – if it is "remote", the device twin settings will be applied. However, if it is local, it will check first to see if the user has set a preference for that, and if not, it will fall back to the device twin settings. 

A policy property can be associated with each configuration group. 

**[gmileka]** sync with the work on locking some csps for oems vs. enterprise… 

## 9.2. Swapping in and out sections

Due to the large number of properties available to configure Windows, and the size limitation on the device twin, the device twin might hit its 8kb limit. 

To help alleviate that problem, one idea is to swap out the groups that have been successfully applied and replace them with other configuration. 

While this might work for a small application – this will become a nightmare for a large number of configurations and devices. To alleviate the problem, the back-end has to implement a way to track such transitions. Such a solution will be overly complicated and error prone. 

The way to work around size limitation is probably to create new device twins…  

[ToDo: need to think more about a solution here].

## 9.3. Transient States During Deletions

In some scenarios, a property group might need to be entirely removed from the device twin. The device twin implementation itself does not support that directly.  

To work around this, the parent node can be set to null in the device twin, and then the contents of the parent node can then be re-constructed.

For example: 

<pre>
"desired": 
    "apps": { 
        "appXYZ": { 
            ... 
            }, 
        "appABC": { 
            ... 
            } 
        } 
    } 
} 
</pre>

Figure 9.3.A 

To delete appXYZ above, we would have to set "apps" to null, and then follow that with re-setting "appABC" configuration again. 

The problem arises when an observer takes a snapshot between the above two steps. It would appear as if the "apps" node completely disappeared. 

This is true for both desired and reported properties. 

To solve this, the client that deletes the contents of a parent node (i.e. "apps" in this example), should instead set it to a pre-defined value; something like "refreshing". This will indicate to observers that it is a transient state and they should check again later. 

----

Last Updated 9/6/2018

[Schema](../schema.md)