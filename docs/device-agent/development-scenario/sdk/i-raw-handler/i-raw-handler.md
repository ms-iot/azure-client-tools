# IRawHandler

## Overview

This abstract class defines the interface the agent host expects of a raw handler to implement so that they can communicate.

## Hierarchy

<pre>
IHandler
    IRawHandler
</pre>

## Header and Namespace

<pre>
#include "AzureDeviceManagementCommon/DMInterfaces.h"

// Microsoft::Azure::DeviceManagement::Common
namespace DMCommon=Microsoft::Azure::DeviceManagement::Common
</pre>

## Methods

#### Setup

- [SetHandlerHost()](i-raw-handler-sethandlerhost.md)

#### Reflection

- [GetId()](i-raw-handler-getid.md)
- [GetReportedSchema()](i-raw-handler-getreportedschema.md)

#### Life-cycle Methods

- [Start()](i-raw-handler-start.md)
- [Stop()](i-raw-handler-stop.md)
- [OnConnectionStatusChanged()](i-raw-handler-onconnectionstatuschanged.md)
- [IsConfigured()](i-raw-handler-isconfigured.md)

#### Callback Methods

- [Invoke()](i-raw-handler-invoke.md)

#### Deployment Status

- [SetDeploymentStatus()](i-raw-handler-setdeploymentstatus.md)
- [GetDeploymentStatus](i-raw-handler-getdeploymentstatus.md)
- [GetDeploymentStatusJson()](i-raw-handler-getdeploymentstatusjson.md)

## Related Topics

- [Implementing Json Interfaces](../../plugin-raw-code.md)
- [Device Agent SDK Reference](../reference-sdk.md)

----

[Development Scenario Walk-Through](../../../development-scenario.md) | [Authoring New Plugins](../../developer-plugin-creation.md)
