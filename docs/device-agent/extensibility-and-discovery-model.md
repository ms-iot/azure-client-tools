# Extensibility and Discovery Model

## Overview

The Azure Device Agent provides a secure and robust framework to remotely monitor and configure (i.e. expose) components on an IoT device to the cloud.

This alleviates component owners from having to account for the common infrastructure functionality necessary for cloud connectivity (device identity, authentication with Azure, maintaining the cloud connection recovery and expiry, error reporting to the cloud, and others).

This allows component owners to focus on defining what aspects of their components need to be exposed to the cloud, and writing the minimal amount of code to achieve that (i.e. just the 'wiring').

For several commonly exposed platform components, the Azure Device Agent comes with ready to build/package wiring, so no coding is needed at all.

When building an IoT solution, the OEM defines what scenarios need to be remotely monitored and configured. For each of those scenarios, the OEM can evaluate whether there is an existing wiring they can re-use, or they need to write their own wiring.

This is all achieved by the Azure Device Agent plug-in model.

## The Azure Device Agent Plug-in Model

### The Handler

#### Overview

In order for component to be exposed to the cloud, we need to assign it an identity, and associated properties and/or actions with that identity.
For example, if we are to expose a component that managed reboot scheduling, we can
- Choose the identity to be `rebootInfo`.
- Define two properties `dailyScheduledReboot` and `oneTimeScheduledReboot`.
- Define a single action `reboot`.

The next step is to write code that will tell the Azure Device Agent that we are interested in changes or actions on that identity (`rebootInfo`), and provide code handlers for when the properties change or when the actions are invoked.
This code should also be capable to reflect the current state of the component to the cloud.

This functionality is what is referred to in our documentation as a "configuration handler" or for short just a "handler".

#### Implementation

Since the handler is an entity that Azure Device Agent understands and interacts with, it needs to implement the `IRawHandler` interface. In this scenario, we call the Azure Device Agent the handler's host.

Below is the handler interaction with its host:
- The host instantiates the handler.
- The initial state for all handlers should be 'inactive'.
- The host passes callback interfaces (`IRawHandler` and `IMdmServer`) to the handler.
- The host establishes connection to Azure IoT Hub.
- The host invokes `Start(const Json::Value& config, bool& active)` on all the instantiated handlers. `Start()` is responsible for:
    - Configuring the handler based on the configuration coming from the host. Currently, this configuration comes from the Azure Device Agent configuration file and it is per-handler.
    - Starting any background tasks if needed. This is useful if the handler is to monitor some hardware, for example, and report back changes to the cloud.
    - Reporting any initial state to the cloud.
    - Informing the host (through the `active` out parameter) whether it is active or not.
- The host checks the `active` state when the twin properties for that target handler changes. If the handler is marked inactive, the twin changes are not passed to the handler.
- [ToDo] When does the host invoke the `Stop()` method?

The handler code can reside in various configurations - depending on which plug-in model is used. See `The Plugin` section for more details.

#### Handler Hosting

There are two ways in which a handler can hosted:

1. Static Handlers
    - The handler is part of the Azure Device Agent binary.
    - It is always available - hence the name 'static'.
    - The handler can be instantiated like any C++ object (in this case, `make_shared<>`).
    - Use this model **only** if the functionality at hand is something that you will always want as part of the agent.
    - Crashes in the plug-in binary may cause the agent to crash.

2. Dynamic Handlers
    - The handler is part of another binary - a **plug-in binary**.
    - The plug-in binaries are enumerated at runtime, and then queried for what handlers they implement - hence the name 'dynamic'.
    - The handler can be instantiated through the plug-in factory (typically a `CreateRawHandler(const std::string& id)`).

It is recommended to use dynamic handlers whenever possible.

### The Plugin

#### Overview

A plug-in refers to the optional binary that hosts one or more handlers and can be enumerated and managed by the Azure Device Agent.

There are two models for using a plug-in:

1. Direct Plugins
    - The plug-in is loaded in proc, and the Azure Device Agent has a direct pointer to the created handlers - hence the name 'direct'.
    - Data does not get marshalled/unmarshalled between the agent and handler. This can be beneficial in cases where high data throughput is required.
    - Both the agent and the plug-in must be compiled using the same toolset and SDKs.
    - Crashes in the plug-in binary may cause the agent to crash.

2. Indirect Plugins
    - The plug-in can be loaded in the same process or in a separate process, and the Azure Device Agent talks to it or its handlers through proxies.
    - Data gets marshalled/unmarshalled between the agent and the handlers.
    - The agent and the plug-in can use different toolsets/SDKs.
    - If the plug-in is out-of-proc, crashes will not affect the agent.

It is recommended to use out-of-proc indirect plug-ins whenever possible to maximize the robustness of the agent - of course, after weighing other factors.

An indirect plug-in is implemented as a dlls and can be loaded in-proc or out-of-proc by just changing its manifest file.
For example, to set the DeviceInfo plug-in to load in proc, its manifest should look something like this:

<pre>
{
  "codeFileName": "DeviceInfoPlugin.dll",
  "direct": false,
  "outOfProc": false
}
</pre>

To load it out of proc, its manifest should look something like this:

<pre>
{
  "codeFileName": "DeviceInfoPlugin.dll",
  "direct": false,
  "outOfProc": true,
  "keepAliveTime": 50000
}
</pre>

A direct plug-in not be loaded out-of-proc.

#### Implementation

A direct plugin needs to implement the following:

1. A way to report supported handlers (see `PluginGetHandlersInfo`).
2. A way to create a handler and return a pointer to it (see `PluginCreateRawHandler`).

[ToDo: Add more specifics]

An indirect plugin needs to impelemnt the following:

1. A way to execute code (see `PluginInvoke`).
2. A way to delete buffers allocated by the plug-in (see `PluginDeleteBuffer`).
3. A way for the plug-in to execture code in the host (see `PluginSetReverseInvoke`).
4. A way for the plug-in to delete buffers allocated by the host (see `PluginSetReverseDeleteBuffer`).

[ToDo: Add more specifics]

----

## Related Topics

- [Schema Patterns](schema.md).
- [Plug-in Creation Walk-Through](development-scenario/developer-plugin-creation.md).
- [Development Scenario](development-scenario.md).
- [Handler Enumeration](extensibility-and-discovery-model/handler-enumeration-flow.md).
- [Plug-in Manifest File](reference/plugin-manifest-file.md).

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)