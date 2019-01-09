# 6. Reporting Status

[Schema](../schema.md)

## 6.1. Overview

At any given point, the operator should be able to inspect the status of a device.

The status include:

- What operations are supported on the device.
- Which operations are in progress and which operations have completed.
- For those which have completed, what the outcome is:
    - If success, is compliance satisfied?
    - If failure, what exactly failed?

## 6.2. Unit of Reporting

For each reported *Configuration Group*, there is a sub-group for status - namely, the `__meta` node.

Configuration Group Lists do not have a status of their own - however, the sub items in those lists will each have its own `__meta` node.

## 6.3. Success

If a *Configuration Group* is successfully applied and reported, the reported section will have a '__meta' node with the following content:

<pre>
    "rebootInfo": {
        "dailyRebootTime": "<i>time</i>",
        "singleRebootTime": "<i>time</i>",
        "__meta": {
            "time": "__time__",
            "state": "succeeded"
        }
    }
</pre>

## 6.4. Errors

### 6.4.1. Overview

Errors are any failures encountered while applying the desired state for a given configuration operation. Errors are also anything the client implementation may decide to report as an error even if it is not the result of applying a desired state.  

Examples:

1. The operator configures the devices to download a package and install it. The path to the package is incorrect. This will be reported as an error when the DM client attempts to apply the desired state. 
2. DM client reports an error whenever the disk storage free space drops below a certain amount. 

### 6.4.2. Unit of Error

Some operations rely on a single property and others rely on a group of properties. Errors will be reported per operation.

- For *Configuration Groups* that contain several properties where all of them constitute a single operation, a single error will be emitted under the `__errors` node.
- For *Configuration Groups* that contain several independent properties, when errors occur, the errors will be combined under the `__errors` node and each operation that failed should have a separate entry.

For example, 

- If an error occurs while installing an application, then the error will be reported under that application's node in the reported properties section. Below is an example of a set of properties that constitute a single operation.

<pre>
"reported" 
    "applications_1": {
        "toaster_2": {
            "__meta_3": {
                "state": "failed",
                ...
            },
            "__errors_3": {
                "install_4": {
                    &lt;error description&gt;
                }
            }
        ...
        }
    }
}
</pre>

*install* is the name of the operation that failed.

Figure 6.4.2.A

- If errors occur while applying some of the Windows update policies, each of the errors will be reported under the `windowsUpdates.__errors` node.

<pre>
    "reported": {
        "windowsUpdates_1": { 
            "activeHoursStart": 3,
            "activeHoursEnd": 5,
            "allowAutoUpdates": 3,
            "allowUpdateService": 1, 
            "branchReadinessLevel": 2, 
            ...,
            "__meta_2": { 
                "state": "failed", 
            },
            "__errors_2": {
                "activeHoursStart_3": {
                    &lt;error description&gt;
                },
                "activeHoursEnd_3": {
                    &lt;error description&gt;
                }
            } 
        } 
    } 
</pre>

Figure 6.4.2.B 

### 6.4.3. Error Description

Errors are not predefined because they can originate from any subsystem on the client device.
The goal is to include enough information in the error description to facilitate efficient queries as well as be sufficiently descriptive to help diagnose the cause of the problem.

The following attributes will be defined for each error:

1. The device management context they were raised-in (while configuring app xyz, for example). 
2. The source sub-system (i.e. OS error, device management client error, etc). 
3. The error code (file not found). 
4. The non-localizable error message. This is important for debugging.
5. The error parameters (i.e. the name of the file that isn't found, etc).

This information will allow us to run queries for very specific errors, get a concise list of devices, and apply the necessary fixes for these particular errors. See the section titled ‘Addressing Issues’ below.

Example:

| Property Name | Property Value  |
|---------------|-----------------|
| Sub-system    | "DeviceManagementClient" |
| Context       | "InstallingPackage" |
| Error Code    | 123 – "Invalid package name $param0". |
| Error Message | "Invalid package name." |
| Parameters    | appXYZ.appx |

<pre>
"reported":
    "windowsUpdates_1": { 
        "activeHoursStart": 3,
        "ActiveHoursEnd": 5,
        "allowAutoUpdates": 3,
        "allowUpdateService": 1, 
        "branchReadinessLevel": 2, 
        ...,
        "__meta_2": { 
            "state": "failed"
        },
        "errors_2": {
            "activeHoursStart_3": {
                "subsystem": "os",
                "context": "set",
                "code": 123,
                "message": "invalid format.",
                "parameters_5": {}
            },
            "allowUpdateService_3": {
                "subsystem": "os",
                "context": "set",
                "code": 123,
                "message": "invalid value.",
                "parameters_5": {}
            }
        }
    },
    "apps_1": { 
        "appXYZ_2": { 
            "__meta_3": { 
                "time": "2018/02/7T:01:10:00", 
                "state": "pending|compliant|nonCompliant|failed" 
            } ,
            "__errors_3" {
                "install_4": {
                    "subsystem": "os",
                    "context": "installingPackage",
                    "code": 123,
                    "message": "file not found.",
                    "parameters_5": {
                        "fileName": "appXYZ.appx"
                    }
                }
            }
        } 
    } 
} 
</pre>

Figure 6.4.3.A

#### Error Look-up

| SUBSYSTEM          | MEANING |
|--------------------|---------|
| "os"               | Error originated from/reported by the OS |
| "deviceManagement" | Error originated from/reported by Device Management |

| CONTEXT   | MEANING |
|-----------|---------|
| ...       | ...     |
| "installingPackage" | Installing package |
| ...       | ...     |

| SUBSYSTEM | CODE    | PARAM1       | MEANING (LOCALIZABLE)        |
|-----------|---------|--------------|------------------------------|
| 1         | OS      | Package file | Invalid package name $param0 |

## 6.5. Compliancy

### 6.5.1. Overview

Compliancy is the state where the device current state matches the device desired state.  

It is measured after a configuration operation is complete (whether it was successful or not). 

Compliancy issues can be the result of errors, however, they can also arise from other conditions. 

Examples: 

1. A device reports the right expected values even though an error occurred during application. The device is considered compliant in this case. 
2. A device reports an unexpected value even though no errors were reported while applying the configuration. The device is considered non-compliant in this case. 
    1. This also could happen if the property is set by means other than the device client (i.e. without its knowledge). 

In the device twin below, note that the reported ActiveHoursStart is different from the desired value.
The device client can be implemented in such a way to flag this as a compliancy issue (note that there is no direct correlation between desired and reported properties. That's why the device client has to implement the correlation for this scenario to be flagged as a non-compliant scenario).

<pre>
"desired": { 
    "WindowsUpdate": { 
        "ActiveHoursStart": 6, 
        "ActiveHoursEnd": 23 
}, 

"reported": { 
    "WindowsUpdate": { 
        "ActiveHoursStart": 3, 
        "ActiveHoursEnd": 23 
    } 
} 
</pre>

Figure 6.5.1.A

### 6.5.2. Correlation

Since compliancy relies on comparing the desired state to the current state (reported or not), there needs to be a mechanism by which a particular desired property (or properties) is correlated with the current state. 

That association and comparison can either occur on the device client itself or in the cloud. 

1. On the cloud, this will require the device to send the necessary data, and rules, to a cloud app for processing. 
2. On the device, this will use the local processing power, distributed by its very nature, and will save portal implementations from having to do that kind of processing and logic on their side. 

We will be going with #2 above – where the client implementation will be responsible for verifying the compliancy at the individual Configuration Group level and aggregating the results up for efficient discovery. For more information about aggregation see the section titled "Addressing Issues". 

**[gmileka]** Consider having compliancy and errors reported as part of the Windows telemetry. 

While the correlation will be implemented by the client, the published schema will indicate the correlation so that portals can implement the necessary visualization. 

The correlation will be in the form of a map of desired properties to reported properties. 

For example:

<pre>
"applications": { 
    "toaster": { 
        "version": "2.0.0", 
        "appxPackage": "toaster.2.0.0.appx", 
        "container": "arm", 
        "startup": false 
    } 
} 
</pre>

Figure 6.5.2.A

The compliance for this configuration can be defined as:

<pre>
desired.applications.&lt;appid&gt;.version == reported.application.&lt;appid&gt;.version 
</pre>

Figure 6.5.2.B

### 6.6.3. Reporting Non-Compliancy

Compliancy will be reported as an error with a subsystem value set to "compliancy".

An error in application will take precendence over compliancy. For example, compliancy issues are only reported if no errors are reported.

##  6.7. Addressing Issues

### 6.7.1. Overview

A typical error drill down looks like this:

1. Detect the presence of an issue.
2. Retrieve more details about the issue.
3. Enumerate all devices affected by a specific issue.
4. Apply a single remedy to all affected devices for a given issue.

The previous sections describe how the data necessary to detect an error or a compliance issue are stored in the device twin. However, the layout described above does not allow an efficient way to query a large number of devices where each device has a large number of properties.  

We can solve these problems by simply aggregating the state of the device into a property or two that can be easily queried.

Once the list of devices is filtered on these properties, the operator can investigate further by looking for more details in the device twin of one of the devices, and then finding similar devices (this flow can also be automated by the back-end portal or a micro-service). 

The aggregation can be done by the device management client on the device itself. This distributes the computation load, reduces the network traffic, and also appropriately has the platform implementation define what constitutes compliancy or not. 

For example:

<pre>
"reported": {
    "__summary": { 
        "failedCount": 3, 
        "failedGroups": "apps:appXYZ;appABC|windowsUpdate",
        "time": "&lt;time&gt;"
    }
}
</pre>

Figure 6.7.1.A

- **[gmileka]** think more about whether appXYZ;appabc are useful to mention or the same number of queries will have to be performed anyway. 
- **[gmileka]** Such summaries (as `failedGroups` above) are useful in running queries that extract the data without having to retrieve the full device twin.

In the above schema, note that we can easily find the devices with either errors or compliance issues by writing something like:

<pre>
SELECT deviceId, __summary.failedCount, __summary.failedGroups
FROM devices
WHERE reported.__summary.failedCount!=0
</pre>

Figure 6.7.1.B

*This satisfies requirement #1.*

The operator will be represented with a list of devices that have errors as a result of running the query above.

The next step is to narrow down the list of device to a specific error...

#### Scenario 1: no back-end processing 

By selecting `reported.__summary.failedGroups` in the previous query (Figure 6.7.1.B), it is easy to spot which Configuration Groups have errors and narrow down the list of devices to single group by writing something like this:

<pre>
SELECT * FROM Devices
WHERE reported.apps.toaster.__meta.status='failed'
</pre>

Figure 6.7.1.C

Then, the operator selects one of the devices in the returned list, and inspects its device twin looking for more details on the errors. 

The `failedGroups` value has information about which sections have failures (i.e. `apps` and `windowsUpdate` in figure 6.6.1.A).  

**[gmileka]** It would be great if you can retrieve sections of the device twin to optimize network traffic and minimize the cost - by retrieving only the sections that have errors. However, IoT Hub does not support that now. Note that a work around is to write a query that pulls the pieces you need.

*This satisfies requirement #2.*

Once an error has been identified, we can write something like the query below to find all affected devices:

<pre>
SELECT * FROM Devices  
WHERE reported.apps.appXYZ.__meta.error.code == 123
</pre>

Figure 6.7.1.D

*This satisfies requirement #3*

Now that we have the list of devices affected with the same exact error, we can author a new configuration, and deploy it to all the devices in that list.

*This satisfies requirement #4*

**[gmileka]** This is actually incorrect. If we attempt to target devices based on their error state, the targeting will disappear once the issue is fixed - which means the device will fall back to its older configuration and error again.
This will result in a cyclic pattern where the device bounces between the error'ed state and the non-error'ed state.
The appropriate fix is to address the problem at the source. Below are possible sources of errors and how to fix them:

| Problem | Fix |
|---------|-----|
| Incorrect configuration deployed. | Update the configuration and re-deploy. |
| Incorrect targeting. | Create a new/use another configuration and target the failing devices. |
| Missing configuration. | Create a new/use another configuration and target the failing devices. |
| Transient errors. | The device client should be robust enough to handle such cases without propagating to the device twin. |
| Others? | ? |

Table 6.7.1.D

#### Scenario 2: back-end processing 

In the previous scenario, the operator had to go inspect one of the devices that had errors, then look at one of the errors, find similar devices, and then apply a fix. 

This might be acceptable if devices share the same error. But as devices report different errors, this scenario becomes very cumbersome. 

A much better operator experience is to see a list of the errors reported in the system, and then s/he selects the most critical and start fixing that first. 

To allow such view, an automation need to start traversing the device twins aggregating this data and then present it to the user. 

This is cross device aggregation, so it has to be done by the back-end (potentially a micro-service). 

**[gmileka]**: Is there a better way? Need to think more.  

This error handling experience is the target experience we should aim for. 

**[gmileka]** make sure this aligns with the **Configuration Management** service. 

**[gmileka]** Note that fixing errors should always be deployment driven. No errors should be fixed with surgical fixes - instead, the originating deployment needs to be fixed (see table 6.7.1).

----

Last Updated 9/6/2018

[Schema](../schema.md)