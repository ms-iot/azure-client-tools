# Repo Build

## First Time Build

The following command builds both the Azure C-SDK and the all the bits in this repo a single architecture and single flavor (debug or release).

- Open the VS command prompt and run:

<pre>
cd / d:\git\dm\scripts
build.cmd x64 Debug
</pre>

## Building Code Changes

Depending on the scenario...

- For coding and fast iteration on building/testing, open the `code\AzureDeviceManagement.sln` and build the flavor/architecture you need.
- For verification before check-in, run `scripts\build.azure-dm.check-in.cmd` from the Visual Studio command prompt.
- For re-building after pulling a new version of the Azure C-SDK, run `scripts\build.azure-c-sdk.check-in.cmd` from a Visual Studio command prompt.

----

[Development Scenario](../development-scenario.md)
