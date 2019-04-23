# Reference

## Capabilities

- [Schema Tables](schema-tables.md)
- [Executable Commands](executable-commands.md)

#### Platform Management Capabilities

- [Device Info](schema/configuration-groups/device-info.md)
- [Certificate Management](schema/configuration-groups/certificate-management.md)
- [Diagnostic Logs Management](schema/configuration-groups/diagnostic-logs-management.md)
- [Factory Reset Command](schema/configuration-groups/factory-reset-cmd.md)
- [Reboot Management](schema/configuration-groups/reboot-management.md)
- [Remote Wipe command](schema/configuration-groups/remote-wipe-cmd.md)
- [Time Management](schema/configuration-groups/time-management.md)
- [Windows Telemetry Management](schema/configuration-groups/windows-telemetry-management.md)
- [Windows Update Management](schema/configuration-groups/windows-update-management.md)

#### DM Client Management Capabilities

- [Device Schemas](schema/configuration-groups/device-schemas.md)
- [Clear Reported Properties](schema/configuration-groups/clear-reported-cmd.md)

#### Servicing

- [Servicing](servicing.md)

#### Common Objects

- [Device Schema Object](schema/configuration-groups/device-schema-object.md)
- [Meta Data Object](schema/configuration-groups/meta-object.md)
- [Error Object](schema/configuration-groups/error-object.md)
- [Summary Object](schema/configuration-groups/summary-object.md)

## Configuration

- [Device Agent Configuration File](reference/device-agent-configuration-file.md)
- [Plugin Manifest File](reference/plugin-manifest-file.md)
- [Device Agent Deployment Layout](reference/device-agent-deployment-layout.md)

## OEM Device Setup

- [OEM Device Setup](oem-device-setup.md)

## Tools

- [DMMockPortal](../dm-mock-portal/dm-mock-portal.md) - For bulk updates of devices
- [DeviceExplorer](reference/device-explorer.md) - For IoT Hub device operations
- [DMValidator](../../code/Tools/DMValidator/Readme.md) - Run test scenarios on devices running DMClient

## Architecture

### Cloud Interface

- [Schema Language](schema.md)
- [Capability Listing Model](capability-listing-model.md)

### Device Agent

- [Device Provisioning Service Integration](dps-integration.md)
- [Connectivity Management](connectivity-management.md)
- [Deployment States](deployment-states.md)
- [Twin Aggregation](twin-aggregation.md)

### Handler Authoring

#### Twin Concepts and Schema Design

- [Configuration Granularity](configuration-granularity.md)
- [Incremental Updates and Merging](incremental-updates-merging.md)
- [Correlating Desired and Reported Properties](correlating-desired-and-reported-properties.md)
- [Deleting Configuration from the Twin](deleting-configuration-from-the-twin.md)

#### Coding Concepts and Design

- [Code Extensibility and Discovery Model](extensibility-and-discovery-model.md)
- [Handler Enumeration and Initialization Flow](extensibility-and-discovery-model/handler-enumeration-flow.md)
- [Handler Active/Inactive States](active-groups.md)
- [Handler Start-up Configuration](handler-start-up-configuration.md)
- [Handler Invoke and Reporting Model](handler-invoke-and-reporting-model.md)
- [Handler Logging](handler-logging.md)

#### Plug-in SDK

- [Create a New Plugin as a Standalone Project](development-scenario/plugin-build-standalone.md)
- [Create a New Plugin to The Agent Solution](development-scenario/plugin-build-with-agent.md)
- [Device Agent SDK Reference](development-scenario/sdk/reference-sdk.md)

----

[Home](../../README.md) | [Device Agent](device-agent.md)
