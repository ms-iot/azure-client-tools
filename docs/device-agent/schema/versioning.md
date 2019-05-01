# 5. Versioning

[Schema](../schema.md)

## 5.1. Overview

Different parts of the device agent are versioned to ensure that they communicate with each other in a compatible way.

## 5.2. Handler Interface Versioning

A plugin can have multiple handlers that it implements to execute different functions. 

The service/operator communicates(reads/writes) with the device using a predetermined set of properties (with a specific structure) called the **interface** 

Given that each side (service and client) is expected to evolve independently, each side has to declare what interface it can understand.

This can be achieved using interface version numbers - where each version number refers to a specific set of properties with a specific structure.

This is not the implementation version of the given handler but the version of the contract that the service and the device side communicate in.

**InterfaceVersion** is a semantic version number that represent the maximum input version of the interface the plugin handler can conform to. The handler is always assumed to be backward compatible.

The plugin author is responsible for ensuring this. In case the author chooses not to be backward compatible, they should handle it accordingly and throw an error.

### 5.2.1.  Versioning Numbers

We will use `major.minor.patch` versioning format.

- `major` changes if new required properties are added, removed or property type is changed.
- `minor` changes if new optional properties are added, removed or property type is changed.
- `patch` changes if there is a small fix in the property definition which is considered as a non-breaking change in the handler. Example: If a property is an enum of colors and you want to add a new color to the enum, it will be considered as a patch change.

### 5.2.2.    Programming implementation: Plugin handler versioning

The plugin handler implements the interface versioning. The handler defines the interface version as a constant.

This is the maximum version of the interface that the handler has conformed to. The plugin developer can choose to make the handler backward compatible.

It checks if it can execute the handler with the desired properties it has received by comparing the service interface version with its interface version.

If succeeded it report the properties and the output version of the interface. If fails, it throws an exception. 

<pre>
    constexpr char InterfaceVersion[] = "3.0.0";
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
            
            string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

            //Compare interface version with the interface version sent by service
            //Here, the solution is backward compatible. The plugin developer can choose not to be backward compatible by proceeding to execute the handler only if the major versions are the same
            if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) >= 0)
            {
                //call on to different version implementations
                if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
                {
                    //execute interface version(3.0.0)
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
                    _metaData->SetDeviceInterfaceVersion(InterfaceVersion);
                }
                else if(MajorVersionCompare("1.0.0", serviceInterfaceVersion) == 0)
                {
                    //execute interface version(1.0.0)
                }
                else if(MajorVersionCompare("2.0.0", serviceInterfaceVersion) == 0)
                {
                    //execute interface version(2.0.0)
                }
            }
            else
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_INTERFACE_VERSION, "Service solution is trying to talk with Interface Version that is not supported.");
            }
        });

        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }
</pre>

### 5.2.3.  Device Schema reporting handler version

A device client can be instructed to report the supported interface version by the following construct:

<pre>
{
    "properties": {
        "desired": {
            "__deviceSchemas": {
                "reporting": {
                    "groups": "all|configured;&lt;group1&gt;;&lt;group2&gt;",
                    "types": "all|raw",
                    "tags": "&lt;tag1&gt;;&lt;tag2&gt;"
                }
            }
        },
        "reported": {
          "__deviceSchemas": {
                "rebootInfo": {
                    "type": "raw",
                    "tags": "dm;sensor",
                    "interfaceVersion": "1.0.0"
                },
                "apps": {
                    "type": "raw",
                    "tags": "dm;sensor",
                    "interfaceVersion": "1.0.0"
                }
            }
        }
    }
}
</pre>

For more details, see [__deviceSchemas](configuration-groups/device-schemas.md).

This allows the following discovery scenarios by the operators:

- Operator wants to target all devices with rebootInfo.interfaceVersion="1.0.0" with a new deployment.
  - The operators create a new deployment in ADM, and sets the targeting condition to:
    <pre>
        SELECT * FROM devices WHERE properties.reported.__deviceSchemas.rebootInfo.interfaceVersion="1.0.0"
    </pre>

- Operator wants to reboot all devices that have cameras and can communicate at a particular interface version.
  - The operator sets the desired state to:
    <pre>
    {
        "properties": {
            "desired": {
                "__deviceSchemas": {
                    "reporting": {
                        "groups": "camera",
                        "types": "all",
                        "tags": ""
                    }
                }
            },
        }
    }
    </pre>

  - The operator creates a new reboot job and sets the targeting condition to:
    <pre>
        SELECT * FROM devices WHERE properties.reported.__deviceSchemas.camera.interfaceVersion="1.0.0"
    </pre>

  - The operator wants to inspect a specific device and know what camera interface it exposes.
    <pre>
        {
            "properties": {
                "desired": {
                    "__deviceSchemas": {
                        "reporting": {
                            "groups": "all",
                            "types": "all",
                            "tags": "camera"
                        }
                    }
                },
            }
        }
    </pre>

    And then, the operator can inspect the reported properties of that device.

### 5.2.4. The Interface Property Group

To fully describe the schema versions as explained above, several fields need to be defined - see the tables below:

#### In the Desired Context

The portal expresses itself using:

| Property Name     | Type | - |Description     |
|-------------------|------|---|----------------|
| serviceInterfaceVersion | string | Capability | The service sends the desired state properties in service interface version mentioned here. The handler needs to be compatible to service the request or it throws an error. |

Table 5.2.4.A

<pre>
{
    "properties": {
        "desired": {
            "rebootInfo": {
                "__meta": {
                    "serviceInterfaceVersion": "1.0.0"
                }
            }
        }
    }
}
</pre>
Figure 5.2.4.B

In the above example (Figure 5.2.4.B):

-The service writes the desired state to the device in interface version 1.0.0. The handler version needs to be 1.0.0 or greater. It should be compatible to process the service request coming in from the cloud.

#### In the Reported Context

| Property Name     | Type | - | Description     |
|-------------------|------|---|-----------------|
| outputInterfaceVersion | string | State | Device writes reported properties using schema *outputInterfaceVersion*.|

Table 5.2.4.C

<pre>
{
    "properties": {
        "reported": {
            "rebootInfo_2": {
                "__meta": {
                    "outputInterfaceVersion": "1.0.0"
                    }
                }
            }
        }
    }
}
</pre>
Figure 5.2.4.D

In the above example (Figure 5.2.4.D):

- The device writes the reported properties in interface version 1.0.0. The service should read the reported configuration in interface version 1.0.0.

## 5.3. Content Versioning

When the operator creates a new set of values for a given set of interface values, she can give it a name to easily identify it in the future (for example, WiFi_v2).
The `deployment` field holds that identity.

Each interface group for a specific handler will have its own `deployment` field.

For example:

<pre>
"desired": {
    "rebootInfo": {
        "dailyRebootTime": "time",
        "singleRebootTime": "time",
        "__meta": {
            "deployment": "rebootInfo_v2"
        }
    }
},
"reported": {
    "rebootInfo": {
        "dailyRebootTime": "time",
        "singleRebootTime": "time",
        "__meta": {
            "deployment": "rebootInfo_v2"
        }
    }
}
</pre>

Figure 5.3.A.

## 5.4.  Device Agent-Plugin Protocol versioning
 
The plugin developer will write a plugin following the specifications listed out by the device agent it is being developed against. This implementation might change leading to plugins not working.

To ensure the compatibility of the plugins with the device agent, versioning is necessary. 

### 5.4.1  Versioning Numbers

We will use `major.minor.patch` versioning format.

- `major` breaking changes
- `minor` non-breaking changes
- `patch` non-breaking patch fixes

### 5.4.2  Versioning implementation

The plugins have the “AgentPluginProtocolVersion” as a part of their manifest files. 
Example of versioning in plugin manifest:

<pre>
{
"AgentPluginProtocolVersion": "1.5.0"
}
</pre>

The device agent should have the same agent-plugin protocol major version as mentioned in the manifest. The protocol version is not backward compatible. 
If the plugin is developed against a different major version then the device agent, then the plugin handlers will not work. 

## 5.5. IoT Hub Revision

### 5.5.1. Overview

*IoT Hub Revision* is used to mark when each individual property (or one of its children) was last updated in IoT Hub. This is maintained by IoT Hub and is persisted in the device twin meta data. 

Revision is designed to determine whether an incoming change notification has been handled before or not. 

For example, a client may have handled: 

<pre>
    Property A @ revision 3 
    Property B @ revision 4 
</pre>

When the client is restarted, and asks for the full device twin, the client might receive: 

<pre>
    Property A @ revision 3 
    Property B @ revision 5 
</pre>

This allows the client to skip re-applying property A, but still applies property B. This is even more efficient when done at the higher levels of the hierarchy, so the client does not have to re-process each individual child property. 

### 5.5.2. Deployments vs. Revisions

Deployment identities can be used to track what in the system lead to the current state - independent of whether that is the first change or the nth change.

Revisions do not maintain the identity of the change and cannot be tracked back to where the current state came from.

----

[Schema](../schema.md)