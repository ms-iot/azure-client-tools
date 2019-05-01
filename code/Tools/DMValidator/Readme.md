# DMValidator

DMValidator is a CLI tool that tests DM Client running on the devices connected to Azure IoT Hub.

## Quick Start

1. Setup the Device Agent on the device where the testing will take place.
2. On the dev box, construct a configuration file with the tests you want to run.
    <pre>
    {
        "connection-string" : "&lt;enter_iothub_owner_connection_string_here&gt;",
        "storage-connection-string": "&lt;enter_azure_storage_connection_string_here&gt;",
        "scenarioFiles": [
            "..\\Common\\TestCases\\DeviceInfo\\DeviceInfo.MBM.00.json"
        ],
        "devices" : [
            "&lt;device_name&gt;"
        ],
        "log" : "e:\\temp\\dmtest"
    }
    </pre>

    - Replace everything in angle brackets with the actual values that apply to your Azure services.
3. Start a Visual Studio command prompt and run the following:
    <pre>
    cd code\Tools\DMValidator\CLI\
    dotnet bin\Debug\netcoreapp2.1\DMConsoleValidator.dll -C c:\tests\validator.config
    </pre>


## Prerequisites

To run the tool, you need to download .NET Core (SDK + CLI)
either using package manager, or compiling from the source.
Below are instructions on how to get the .NET Core SDK:

Try the installation instructions from this website:
https://www.microsoft.com/net/learn/dotnet/hello-world-tutorial

## DMValidator configuration file

Passing the same values every time you need to use DMValidator can be cumbersome.
You can use JSON configuration file instead. The default location for the configuration
file is the current working directory and the default name is `DMValidator.json`.

### Configuration file  vs. command line arguments priority

When DMValidator runs, it first uses the options passed via command line arguments,
and if there are any options that have not been provided, DMValidator will
use configuration file to determine the value. If both methods fail,
DMValidator will use default values (or signal an error if a value needs to be provided).

### Turning off configuration file

DMValidator will skip using configuration file if `-n`(`--no-config`) flag is provided.

### Configuration file at another location
If the configuration file is stored at a location other than default,
you can use `-C`(`--CONFIG`) flag to point to a configuration file (see Examples).

### Configuration file layout

Configuration file is a JSON file. All values are stored in top-level JSON object. Key names are long flag names.
All options use strings as their values, unless more than one value can be passed to the flag (for example, `-d` flag accepts more than one device, separating them using comma). If there are more values expected, the values
have to be passed as an array (even if providing just one value).

### Example configuration file

<pre>
{
    "connection-string" : "enter_iot_hub_connection_string",
    "scenariosFolder" : "..\\Common\\TestCases\\TimeInfo\\",
    "scenarioFiles": [
        "..\\Common\\TestCases\\DeviceInfo\\DeviceInfo.MBM.00.json"
    ],
    "devices" : [
        "device_name"
    ],
    "log" : "e:\\temp\\dmtest"
}
</pre>

## Examples

### Reading DMValidator help message

To print DMValidator's help message, use `-h` flag:

`dotnet run -- -h`

### Running DMValidator for all devices|tests

In this example we run DMValidator for all devices. We assume that the connection string is `secret;string`.

`dotnet run -- -c 'secret;string'`

### Running DMValidator for selected devices

In this example, we only want "Device1" and "OtherDevice" to run tests.

`dotnet run -- -c 'secret;string' -d 'Device1,OtherDevice'`

### Listing devices connected to azure IoT Hub

`dotnet run -- -c 'secret;string' -l`

### Running DMValidator for tests located in other folder

Assuming tests are stored in c:\dm\othertestdir\

`dotnet run -- -c 'secret;string' -t c:\dm\othertestdir\`

### Running DMValidator with different log destination

In this example we want to store log files in `logs` folder in our dm directory

`dotnet run -- -c 'secret;string' -L "c:\dm\logs"`

### Running DMValidator with configuration file and flags

In this example we assume that we have a configuration file with connection string, but we want to pass a different log destination.

`dotnet run -- -L "c:\dm\logs"`

### Running DMValidator without using a configuration file

In this example we ignore a configuration file and pass arguments using command line.

`dotnet run -- -n -c 'secret;string' -t c:\dm\othertestdir\`

### Running DMValidator using a configuration file at non-standard location

<pre>
cd code\Tools\DMValidator\CLI\
dotnet bin\Debug\netcoreapp2.1\DMConsoleValidator.dll -C c:\tests\validator.config
</pre>

where validator.config has something like:

<pre>
{
    "connection-string" : "&lt;enter_iothub_owner_connection_string_here&gt;",
    "storage-connection-string": "&lt;enter_azure_storage_connection_string_here&gt;",
    "scenariosFolder" : "..\\Common\\TestCases\\TimeInfo\\",
    "scenarioFiles": [
        "..\\Common\\TestCases\\DeviceInfo\\DeviceInfo.MBM.00.json"
    ],
    "devices" : [
        "&lt;device_name&gt;"
    ],
    "log" : "e:\\temp\\dmtest"
}
</pre>

## Notes

### NU1701 "Package 'X' was restored..."

This warning is related to the usage of .NET Framework libraries and .NET Core 2 tools.

If .NET Framework is not present, .NET Core 2 programs can reference .NET libraries,
although there is no guarantee that such a library will work - the failure might happen at the runtime.

The warning is silenced in .csproj file.

----

[Home](../../../README.md) | [Device Agent](../../../docs/device-agent/device-agent.md) | [Reference](../../../docs/device-agent/reference.md)
