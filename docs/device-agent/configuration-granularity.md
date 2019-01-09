# Configuration Granularity

Given a platform component that we want to expose to the cloud, we will need to define various states and some relationships between them.

We will use configuration applications as an example to demonestrate the concepts at hand.

## The Configuration Group

### The Concept

A configuration group is a set of properties that describe the behavior or a policy for a platform component. A configuration group is described in the twin by a top-level json node - in both the desired and the reported property sections.

<pre>
{
    "properties":{
        "desired":{
            "configurationGroup_A":{
            },
            "configurationGroup_B":{
            }
        },
        "reported":{
            "configurationGroup_A":{
            },
            "configurationGroup_B":{
            }
        }
    }
}
</pre>

### The Implementation: The Handler

A handler is the code responsible for applying the configuration group to the underlying platform component. The same handler is also responsible for reporting the state of that platform component.

A handler can be responsible for only a single configuration group.

## The Configuration Sub-Group

### The Concept

A configuration sub-group is a subset of the configuration group properties that constitute a coherent configuration. In otherwords, the properties of the sub-group are the properties that contribute to a final desired state.

For example, to install an application from an external storage, we would need three attributes:

- application key
- version number
- extranal storage

The three attributes together contribute to the successful installation of the application. In this case, the sub-group has three properties.

In contrast, configuring the application to be a start-up application can be represented with a single attribute, and also notice that it is independent of the other attributes:

- startup

In this case, the sub-group has a single property.

A sub-group constitutes the atomic unit of state update. 

If an error occur while processing a sub-group, processing should stop and the error should be reported.

The reported error will be attributed to the sub-group context.

Also, in most cases, processing should not stop if one sub-group fails.

For example, 

<pre>
{
    "properties":{
        "desired":{
            "apps":{
                "appXYZ": {
                    "key": "xyz",
                    "version": "1.0",
                    "storage": "url://",
                    "startup": true
                }
            },
            "configurationGroup_B":{
            }
        },
        "reported":{
            "apps":{
                "appXYZ": {
                    "__errors": {
                        "install": {
                            &lt;error description&gt;
                        },
                        "startup": {
                            &lt;error description&gt;
                   
                        }
                    }
                }
            }
            "configurationGroup_B":{
            }
        }
    }
}
</pre>

Notice that we have:
- one error for the 'install' - which results from processing the sub-group {key, version, storage}.
- one error for the "startup' - which results from processing the sub-group {startup}.

### The Implementation: The `RunOperation()`

To Help ease implementing this concept, we are providing the `RunOperation()` method which takes a lambda.

The lambda code is the sub-group handler code. Should any exception be thrown, `RunOperation()` will catch it and convert it into errors to be reported to the twin later.

Each independent piece of code in a handler must be contained in a `RunOperation()`. This means that it is possible to next `RunOperation()` should you have scenarios where the code processing the sub-groups can also be split into independent chunks.

The pattern looks like this:

<pre>
    Invoke(Json::Value groupConfiguration)
    {
        RunOperation(reportedObject, errors [](){

            // Parse/Locate Sub-Group 0 Parameters
            ...

            // Merge
            ...

            // Apply
            SetSubGroup(...);

            if (report == true)
            {
                // Report
                GetSubGroup(...);
                Report(...);
            }
        });

        RunOperation(reportedObject, errors [](){

            // Parse/Locate Sub-Group 1 Parameters
            ...

            // Merge
            ...

            // Apply
            SetSubGroup(...);

            if (report == true)
            {
                // Report
                GetSubGroup(...);
                Report(...);
            }
        });
    }
</pre>

## Groups vs. Sub-Groups

#### Why don't we make all configuration sub-groups into configuration groups?

Technically, we can. However, given that many sub-groups are signle-property, there is a higher chance for name collisions if the list if flattened. On the point, group allows us to conserve space by not having to prefix each property name with its logical group name (as a work around to avoid name collisions).

Also, having configuration groups be reasonably populated with sub-groups allows a more usable model of composability and code ownership management. 
For example, we can have a handler for all application management scenarios - and it is easy to associate the code with the json. 
This is critical to easily maintain schema versioning and backward compatibility. 
Instead of having a schema for each individaul field, we can make promises at a group level.

#### Why aren't sub-groups represented explicitly in the json tree?

To save nesting levels. IoT Hub device twin allows only up to 5 nesting levels and we need to stay within those boundaries.

The 5 levels are current used as follows:

<pre>
{
    "properties":{
        "desired":{
            "configuration_group_id_1":{
                "instance_id_2": {
                    "property_3": "xyz",
                }
            }
        },
        "reported":{
            "configuration_group_id_1":{
                "instance_id_2": {
                    "__errors_3": {
                        "context_4": {
                            "parameters_5": {
                            }
                        }
                    }
                }
            }
        }
    }
}
</pre>

## Optional Sub-Groups and Optional Properties

Given that each configuration sub-group is independent of the other, the presence of one does not impact whether another configuration sub-group must be present too or not.

The presence of any configuration sub-group in its parent configuration group should typically be optional or not applicable. 

### Optional Sub-Groups

Requiring the presence of a configuration sub-group might be necessary since its absence simply implies a no-op and treating that as an error is unnecessary overhead.

For example, assuming that `rebootInfo` has a sub-group of one property called `dailySchedule`, the following desired configurations should be allowed:

<pre>
{
    "properties": {
        "desired": {
            "rebootInfo": {}
        }
    }
}
</pre>

<pre>
{
    "properties": {
        "desired": {
            "rebootInfo": {
                "dailySchedule": ""
            }
        }
    }
}
</pre>

### Not Applicable Sub-Groups

Some configuration sub-groups might not be applicable in the desired context or in the reported context. For example, a read-only property can only be applicable in the reported section of the twin.
Likewise, the handler author may choose to not report some of the desired sub-groups - in that case, those sub-groups are not applicable in the reported section.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)

