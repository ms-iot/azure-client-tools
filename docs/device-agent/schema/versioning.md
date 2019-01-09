# 5. Versioning

[Schema](../schema.md)

## 5.1. Overview

The device client and the back-end use **Configuration Groups** to communicate through the cloud (Azure's device twin).

Both sides need to understand the schema of such **Configuration Groups** to communicate effectively.

Given that each side is expected to evolve independently, each side has to declare what schema it is using for the data it presents and what schema it can read (presented by the other side).

This can be achieved through the use of schema version numbers - where each version number refers to a specific set of properties with a specific structure.

Another form of versioning is that which deals with the content.

## 5.2. Structure Versioning

### 5.2.1. Unit of Schema Versioning

Each Configuration Group will have its own schema versions - unless it is part of a Configuration Group List where the version information will be at the list level (not the individual element configuration groups).

This implies that all elements of a given Configuration Group List will share the same schema versions.

The absence of a schema version field will cause the Configuration Group to be ignored. 

Example: 

Version 1 of the WindowsUpdate property group schema: 

### 5.2.2. The Schema Property Group

To fully describe the schema versions as explained above, several fields need to be defined - see the tables below:

#### In the Desired Context

The portal expresses itself using:

| Property Name     | Type | - |Description     |
|-------------------|------|---|----------------|
| maxInputVersion   | int  | Capability | Portal can read reported proeprties in schema *maxInputVersion* or lower. The client should not report using schemas higher than this number. |
| outputVersion     | int  | State | Portal writes desired properties in schema *outputVersion*.|

Table 5.2.2.A

<pre>
{
    "properties": {
        "desired": {
            "__portalSchemas_1": {
                "rebootInfo_2": {
                    "maxInputVersion": "3.0",
                    "outputVersion": "2.0""
                }
            }
        }
    }
}
</pre>
Figure 5.2.2.B

In the above example (Figure 5.2.2.B):

- The portal can read reported properties in schema version 3 or lower. The client should write its output (the portal's input) in schema version 3 or lower.
- The portal writes the desired properties in schema version 2. The client should read the desired configuration in schema version 2.


#### In the Reported Context

| Property Name     | Type | - | Description     |
|-------------------|------|---|-----------------|
| maxInputVersion   | int  | Capability | Device can read desired proeprties in schema *maxInputVersion* or lower. The portal should not configure using schemas higher than this number. |
| outputVersion     | int  | State | Device writes reported properties using schema *outputVersion*.|

Table 5.2.2.C

<pre>
{
    "properties": {
        "reported": {
            "__deviceSchemas_1": {
                "rebootInfo_2": {
                    "maxInputVersion": "4.0",
                    "outputVersion": "5.0"
                }
            }
        }
    }
}
</pre>
Figure 5.2.2.D

In the above example (Figure 5.2.2.D):

- The device can read desired properties in schema version 4 or lower. The portal should write its output (the device's input) in schema version 4 or lower.
- The device writes the reported properties in schema version 5. The portal should read the reported configuration in schema version 5.

Note that there is no correlation assumed between the desired properties and the reported properties of the same configuration. Each can have their own independent schema versioning.

## 5.3. Versioning Numbers

We will use `major.minor` versioning format.

- `major` changes if new required properties are added or removed.
- `minor` changes if new optional properties are added.

For example, a client capable of reading schema version 4.0 can still be sent 4.1 and it should just ignore the additional fields.

## 5.4. Content Versioning

When the operator creates a new set of values for a given *Configuration Group*, she can give it a name to easily identify it in the future (for example, WiFi_v2).
The `deployment` field holds that identity.

Each Configuration Group will have its own `deployment` field.

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

## 5.5. Reporting Device Client Capabilities

A device client can be instructed to report the supported schema version by the following construct:

<pre>
{
    "properties": {
        "desired": {
            "__deviceSchemas_1": {
                "reporting_2": {
                    "groups": "all|configured;&lt;group1&gt;;&lt;group2&gt;",
                    "types": "all|raw",
                    "tags": "&lt;tag1&gt;;&lt;tag2&gt;"
                }
            }
        },
        "reported": {
          "__deviceSchemas_1": {
                "rebootInfo_2": {
                    "type": "raw",
                    "tags": "dm;sensor",
                    "maxInputVersion": "2.0",
                    "outputVersion": "3.0",
                },
                "apps_2": {
                    "type": "raw",
                    "tags": "dm;sensor",
                    "maxInputVersion": "2.0",
                    "outputVersion": "2.0"
                }
            }
        }
    }
}
</pre>

For more details, see [__deviceSchemas](configuration-groups/device-schemas.md).

This allows the following discovery scenarios by the operators:

- Operator wants to target all devices with rebootInfo.version="3.0" with a new deployment.
  - The operators create a new deployment in ADM, and sets the targeting condition to:
    <pre>
        SELECT * FROM devices WHERE properties.reported.__deviceSchemas.rebootInfo.outputVersion="3.0"
    </pre>

- Operator wants to reboot all devices that have cameras.
  - The operator sets the desired state to:
    <pre>
    {
        "properties": {
            "desired": {
                "__deviceSchemas_1": {
                    "reporting_2": {
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
        SELECT * FROM devices WHERE properties.reported.__deviceSchemas.camera.outputVersion="3.0"
    </pre>

  - The operator wants to inspect a specific device an know what camera interface it exposes.
    <pre>
        {
            "properties": {
                "desired": {
                    "__deviceSchemas_1": {
                        "reporting_2": {
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


## 5.6. Migration

When either the device client or the back-end is updated, the question of how other pieces of the system should continue to communicate with the upgraded piece arises.

Given that the solution at hand consists of thousands of devices, and a back-end, there is no possibility that all the pieces will be upgraded at the same time to a certain version.

As a result, the solution must be designed to take into account that different pieces will be running different versions of the client - and the back-end needs to still maintain its ability to communicate with them.

To satisfy this requirement, the newer piece has to continue to support older pieces.

For example,

- If the device client is newer, it has to support reading older desired property schemas and writing older reported property schemas.
- If the back-end is newer, it has to support writing older desired property schemas, and reading older reported property schemas.

## 5.7. IoT Hub Revision

### 5.7.1. Overview

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

This allows the client to skip re-applying property A, but still applies property B. This is even more efficient when done at the higher levels of the hierarchy so the client does not have to re-process each individual child property. 

### 5.7.2. Deployments vs. Revisions

Deployment identities can be used to track what in the system lead to the current state - independent of whether that is the first change or the nth change.

Revisions do not maintain the identity of the change and cannot be tracked back to where the current state came from.

----

Last Updated 9/6/2018

[Schema](../schema.md)