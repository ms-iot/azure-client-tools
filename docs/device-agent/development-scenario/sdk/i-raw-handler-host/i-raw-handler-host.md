# IRawHandlerHost

## Overview

This abstract class defines the interface the handler expects of its host to implement so that they can communicate.

## Hierarchy

<pre>
IRawHandlerHost
</pre>

## Header and Namespace

<pre>
#include "AzureDeviceManagementCommon/DMInterfaces.h"

// Microsoft::Azure::DeviceManagement::Common
namespace DMCommon=Microsoft::Azure::DeviceManagement::Common
</pre>

## Methods

#### Cloud Methods

- [Report()](i-raw-handler-host-report.md)
- [SendEvent()](i-raw-handler-host-sendevent.md)

#### SyncML Support

- [GetMdmServer()](i-raw-handler-host-getmdmserver.md)

## Related Topics

- [Implementing raw Interfaces](../../plugin-raw-code.md)
- [Device Agent SDK Reference](../reference-sdk.md)

----

[Development Scenario Walk-Through](../../../development-scenario.md) | [Authoring New Plugins](../../developer-plugin-creation.md)
