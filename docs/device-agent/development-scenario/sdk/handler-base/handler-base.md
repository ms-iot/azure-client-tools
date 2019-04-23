# HandlerBase Class

## Overview

This class provide default implementation for most of the methods needed by [`IHandler`](../i-handler/i-handler.md) and [`IRawHandler`](../i-raw-handler/i-raw-handler.md).

## Hierarchy

<pre>
IHandler
    IRawHandler
        HandlerBase
</pre>

## Header and Namespace

<pre>
#include "AzureDeviceManagementCommon/HandlerBase.h"

// Microsoft::Azure::DeviceManagement::Common
namespace DMCommon=Microsoft::Azure::DeviceManagement::Common
</pre>

## Methods

### IHandler

- [GetHandlerType()](handler-base-gethandlertype.md)

### IRawHandler

#### Setup

- [SetHandlerHost()](handler-base-sethandlerhost.md)

#### Reflection

- [GetId()](handler-base-getid.md)
- [GetReportedSchema()](handler-base-getreportedschema.md)

#### Life-cycle Methods

- [Start()](handler-base-start.md)
- [Stop()](handler-base-stop.md)
- [OnConnectionStatusChanged()](handler-base-onconnectionstatuschanged.md)
- [IsConfigured()](handler-base-isconfigured.md)

#### Cloud-To-Device Methods

- [Invoke()](handler-base-invoke.md)

#### Deployment Status

- [GetDeploymentStatus()](handler-base-getdeploymentstatus.md)
- [SetDeploymentStatus()](handler-base-setdeploymentstatus.md)
- [GetDeploymentStatusJson()](handler-base-getdeploymentstatusjson.md)

### Helpers

#### Device-To-Cloud Methods

- [SendEvent()](handler-base-sendevent.md)
- [ReportRefreshing()](handler-base-reportrefreshing.md)
- [FinalizeAndReport()](handler-base-finalizeandreport.md)

#### Handler Static Configuration

- [SetConfig()](handler-base-setconfig.md)
- [GetConfig()](handler-base-getconfig.md)

## Related Topics

- [Operation Class](../operation/operation.md)
- [Device Agent SDK Reference](../reference-sdk.md)

----

[Development Scenario Walk-Through](../../../development-scenario.md) | [Authoring New Plugins](../../developer-plugin-creation.md) | [Implementing Json Interfaces](../../plugin-raw-code.md)
