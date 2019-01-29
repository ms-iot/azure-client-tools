# Handler Enumeration and Initialization Flow

## Overview

Below is a high-level description of the agent's flow. The agent:

- Reads its configuration file.
- Enumerates and loads plug-ins.
- Enumerates and instantiates the handlers.
- Connects to Azure.
- Signals the handlers to 'start'.
- Listens to Azure property changes or commands.
- Renews the Azure connection when it expires.

Below is a detailed description of each of these steps.

## Reading The Configuration File

- The Azure Device Agent reads its configuration file.
    - A sample can be found here: [AzureDeviceManagementClient.json](../../../code/AzureDeviceManagementClient/AzureDeviceManagementClient.json).
- The configuration file determines where the plug-in manifests are, and what each handler static configuration is.
    - Static configuration is a json blob that is passed to the handler when it is loaded and started by the device agent (by calling `Start()`).

References
- [Device Agent Configuration File](../reference/device-agent-configuration-file.md).

## Plug-in Loading and Handler Creation

- The Azure Device Agent iterates through each of the plug-in manifests:
    - Determines whether the plug-in is a `Direct` plug-in or not and creates the appropriate proxy.
    - Loads the target plug-in binary.
    - Queries the plug-in for the handlers it implements.
        - A list of handler names and their corresponding types is returned.
    - Iterates through handlers information,
        - Creates each handler and attaches the callback sites.

References
- [Extensibility and Discovery Model](../extensibility-and-discovery-model.md).

## Connecting to Azure

- The Azure Device Agent connects to Azure using a SAS token with an expiry period (by default it is set to 24 hours).
- At this point, individual handlers can start sending data to IoT Hub.

## Signaling `Start`

- The Azure Device Agent iterates through the instantiated handlers,
    - Signals start (by calling `Start`).
        - Passes each handler its static configuration (read earlier from the Azure Device Agent configuration file).
        - The handler can:
            - initialize itself based on the passed-in configuration.
            - Spawn another thread to start back-ground tasks (reading sensors, reporting data, etc).

## Listening to Azure Changes/Commands

- The Azure Device Agent starts listening to Azure changes. Should a property value is changed or a command is sent, the target handler's callbacks will be called.
- While the main loop is listening, handlers which have spawned their own threads can start sending data to the cloud (using reported properties or events).

## Reseting the Connection to Azure

- The Azure Device Agent monitors the SAS token expiry, and once it enters a grace period, it resets the connection.
- When the connection is about to be reset,
    - All handlers are signaled a stop (by calling `Stop`).
    - The connection is re-initialized.
    - All handlers are signaled a start (by calling `Start` with the configuration again).

----

### Related Topics

- [Extensibility and Discovery Model](../extensibility-and-discovery-model.md).
- [Debugging](../development-scenario/debugging.md).

----

[Development Scenario](../development-scenario.md)
