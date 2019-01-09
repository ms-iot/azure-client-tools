# 8. Group Interdependency

[Schema](../schema.md)

In some scenarios, the order of applying settings may affect the final result. For example, to apply WiFi configurations, we need to have Azure storage configuration defined first to be able to download the profile. 

The dependencies can be defined in the `dependencies` property: 

<pre>
{
    "azureStorage": { 
        "defaultContainer": "default", 
        "connectionString": "<connection string>" 
    }, 
    "wifi": { 
        "__meta": {
            "deployment": "RS4_Schema",
            "dependencies": "azureStorage;someOtherGroup"
        },
        "profileName": "MSFTWan"
    }
}
</pre>

Figure 8.A

Note that the physical order of the configuration in the device twin does not determine the actual order of execution. Only the "dependencies" property determines what needs to be processed before the current version. 

----

Last Updated 9/6/2018

[Schema](../schema.md)