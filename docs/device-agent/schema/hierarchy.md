# 3. Hierarchy
[Schema](../schema.md)

## 3.1. Logical Grouping

The expected number of configurable/reported properties for Windows is in the range of 300~500 properties. Operators are not expected to directly manipulate the device twins. Instead, they will use a web portal or an app to manipulate them. Keeping them as a flat list of properties will not provide the best experience for the following reasons: 

- There is increased likelihood of name collisions between different areas.
  - For example, both WiFi configuration and application management require paths to file names to download. They can’t be simply named ‘fileName’ or else there will be a conflict. Instead, and to keep it a flat list, we’ll have to prefix the name; i.e. wifiFileName and appFileName… Even then, this does not scale when we have multiple applications configured (app1FileName, app2FileName, etc). 
- **[gmileka]** Make sure they increase/remove the limit on the nesting level. 
- While we are planning on implementing a web portal, 3rd parties or OEMs are also expected to build their own custom portals to manipulate this schema. A flat list does not communicate the relationship between the various properties and leads to a much more difficult programming experience when it comes to which properties need to be always set together, etc. 
- Also for debugging, it is very convenient to inspect the device twin, and in some cases even modify it. A flat list makes this experience very hard and error prone. 

To solve these problems, we will group the properties by their logical meaning under higher-level properties. Each group will be referred to as a **"Configuration Group"** in this document ([Glossary](glossary.md)). 

<pre>
"windowsUpdate": {  
    "activeHoursStart": 6, 
    "activeHoursEnd": 23 
},
"timeSettings": { 
    "timeZone": "UTC-08:00" 
    "timeServer": "time.windows.com" 
} 
</pre>

Figure 3.1.A 

## 3.2. Unit of Operation

A **"Configuration Operation"** is a command to be run on the device (to change state, apply configuration, etc). 

The property or the properties that are meant to perform a **"Configuration Operation"** on the device will be referred to as **"Operation Properties"** in this document ([Glossary](glossary.md)). 

A given **Configuration Group** may hold properties for one or more operations. We will support the following patterns:

1. A number of properties are logically related, but they can be set/be present independently. In such cases, it does not make sense to place each one of them in a separate Configuration Group, so they are all placed in the same group and the schema indicates they are completely independent (i.e. each property by itself is sufficient for a successful Configuration Operation).

    For example:

    <pre>
    "windowsUpdate": {  
        "activeHoursStart": 6, 
        "activeHoursEnd": 23 
    } 
    </pre>
    Figure 3.2.A

2. A number of properties are logically related, and they must be present at the same time. In such cases, those properties will be grouped under a Configuration Group.
    <pre>
    "applications": { 
        "toaster": { 
            "appxPackage": "toaster.1.0.0.appx", 
            "container": "arm", 
        } 
    } 
    </pre>
    Figure 3.2.B

The schema will indicate the individual operations and which properties are used by each operation. 

## 3.3. Instancing

Some of the configuration areas will need multiple instances of the same configuration structure. A typical example of that is application configuration. A typical device can be configured to have a foreground application and one or more background tasks. Each of them have a name, version, etc. 

One way to represent this is through arrays:

<pre>
"applications": [  
    { "name": "toaster", "version": "1.0.0" }, 
    { "name": "sensorReader", "version": "2.0.0" } 
] 
</pre>

Figure 3.3.A

However, the device twin does not support arrays. So, we have to create new unique identities in our hierarchy. For example:

<pre>
"applications": { 
    "toaster": { 
        "version": "1.0.0" 
    }, 
    "sensorReader": { 
        "version": "2.0.0" 
    } 
} 
</pre>

Figure 3.3.B

The downside to this approach is that:
- The client has to create unique identities.
- Parsing will be more complicated.

While there are downsides, the proposed schema above satisfies our needs and we will define our instance elements using it. It also allows query patterns such as:

`SELECT deviceId FROM devices WHERE applications.toaster.version='1.0.0'`

We will refer to the instances such as those shown above as **"Configuration Group Instances"** - while the group that lists them (i.e. `applications`) will be referred to as **"Configuration Group List"** ([Glossary](glossary.md)).

----

Last Updated 9/5/2018

[Schema](../schema.md)