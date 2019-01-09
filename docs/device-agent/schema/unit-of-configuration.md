# 4. Unit of Configuration

[Schema](../schema.md)

## 4.1. Required vs. Optional

Any given operation will require the presence of a certain set of properties. The operation might make use of additional properties – but they can be optional and default to some predefined values.

<pre>
"applications": { 
    "toaster": { 
        "configId": 1 
        "appxPackage": "toaster.1.0.0.appx", 
        "container": "arm", 
        "startup": false 
    } 
} 
</pre>

Figure 4.1.A 

In the example above, without the "appxPackage" and "container" properties the operation cannot be completed (the package cannot be located). In contrast, if the startup property is not defined, the installation can still take place and assumes that startup is false. 

The published schema should define which properties are required and which properties are optional. 

Both the client implementation and the portal implementation should honor that specification. 

## 4.2. Unit of Setting

The IoT Hub device twin is designed to minimize the payloads it sends to devices by sending only the deltas representing the changes. This is a desired behavior to minimize the network traffic and the cost to our customers. 

IoT Hub implements this by sending only the changed payload along with its parents. In the example above, if the operator is to change "appxPackage" to "toaster.2.0.0.appx", the following snippet will be sent to the device: 

<pre>
"applications": { 
    "toaster": { 
        "appxPackage": "toaster.2.0.0.appx", 
    } 
} 
</pre>

Figure 4.2.A 

This snippet by itself does not contain all the required parameters (it is missing the "container" field). 

However, if the device has a cached version of the required parameters and only updates the one it receives the delta for, the new set of required parameters can be reconstructed (by merging) and used. 

So, applying the contents of Figure 4.2.A to 4.1.A, the client will have all required parameters: 

<pre>
"applications": { 
    "toaster": { 
        "appxPackage": "toaster.2.0.0.appx", 
        "container": "arm", 
        "startup": false 
    } 
} 
</pre>

Figure 4.2.B 

The unit of setting can be as small as a single property – however, if after merging into the cached version on the client, the full set of required properties is not present, the configuration is rejected. 

This can be implemented on the client side as follows: 

- On start-up, a full copy of the device twin is retrieved and cached. 
- As incremental updates are received, they are applied to the cached device twin. 
- Operations are fed from the cached device twin.

**ToDo**
The implementation needs to retain the knowledge of which operations are to be run before merging. This is necessary to avoid re-running all the operations - as opposed to only those that have changed.

----

Last Updated 9/6/2018

[Schema](../schema.md)