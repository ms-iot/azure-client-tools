# SDK Build

- Build all the Azure Device Agent libraries for both debug and release.
    - Start Visual Studio command prompt.
    - Run the following:
    <pre>
    cd scripts
    build.azure-c-sdk.check-in.cmd 10.0.16299.0
    build.azure-storage-cpp.check-in.cmd 10.0.16299.0
    build.azure-dm.check-in.cmd 10.0.16299.0
    </pre>

    The build should take about an hour.

- Prepare the Azure Device Agent headers and libraries to be used by your plug-in project.
    - Using an expanded SDK layout.
        - Create a plug-in SDK folder using something like:
            <pre>
            cd scripts
            deploy.create-sdk-folder.cmd c:\deploy.sdk
            </pre>
    - Using a nuget.
        - Create the nuget using something like:
            <pre>
            cd scripts\nuget
            build.plugin-nuget.cmd 2.0.0
            </pre>

----

## Related Topics

- [Extensibility and Discovery Model](../extensibility-and-discovery-model.md).
- [Handler Enumeration and Initialization Flow](../extensibility-and-discovery-model/handler-enumeration-flow.md).
- [Debugging](debugging.md).

----

[Home](../../../README.md) | [Device Agent](../device-agent.md) | [Development Scenario](../development-scenario.md) | [Plug-in Creation Walk-Through](developer-plugin-creation.md)
