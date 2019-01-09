# DMValidator

DMValidator is a CLI tool that tests DM Client running on the devices connected to Azure IoT Hub.

## Prerequisites

To run the tool, you need to download .NET Core (SDK + CLI)
either using package manager, or compiling from the source.
Below are instructions on how to get the .NET Core SDK:

Try the installation instructions from this website:
https://www.microsoft.com/net/learn/dotnet/hello-world-tutorial

### Libraries

Libraries are pulled automatically by `dotnet` when building.


## Building DMValidator

To build DMValidator, use `dotnet build` command in the DMValidator directory:

```bash
#Assuming we are in the project root directory:
cd code\tests\DMValidator\CLI
dotnet build
```

You can also build the code from any other directory by providing a path to the .csproj file:

```bash
#Assuming we are in the project root directory:
dotnet build code\tests\console\DMValidator\CLI\DMValidator.csproj
```

## Running DMValidator

To run DMValidator, use `dotnet run` command in the DMValidator directory:

```bash
#Assuming we are in the project root directory:
cd code\tests\console\DMValidator\CLI\
dotnet run
```

You can also run the program from any other directory by providing a path to the .csproj file using `--project` flag:

```bash
#Assuming we are in the project root directory:
dotnet run --project code\tests\console\DMValidator\CLI\DMValidator.csproj
```

*NOTE:** This does not change the current working directory, which is used by the project to access files!

### Running and building DMValidator at once

`dotnet run` command builds the program first(if needed), so you can run just this one command.

## Passing arguments to DMValidator

Because we use `dotnet` to run DMValidator, our argument goes through three steps:

- Bash parses the argument
- `dotnet` receives the parsed argument
- DMValidator receives the argument passed by `dotnet`

This means our argument has two places where it could break:

- IoT Hub connection strings contain semicolons ';'. These are recognized by bash as command separators. To avoid getting interpreted by bash, ensure that all the arguments are quoted(single or double quotes).
- `dotnet` might not distinguish between arguments meant for DMValidator and itself. To make sure that arguments are passed to DMValidator, separate arguments to `dotnet` and DMValidator with `--` (if there are no dotnet arguments simply put this string after the dotnet command call. )

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
    "connection-string" : "insert-your-string",
    "tests" : "..\Common\",
    "devices" : [
        "myDevice1",
        "myDevice2"
    ],
    "log" : ".",
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

Assuming that a configuration file is stored in a current working directory:

`dotnet run -- -C otherName.json`

Assuming that a configuration file is stored as 'validator.config' in our dm directory:

`dotnet run -- -C "c:\dm\validator.config"`

or:

<pre>
cd code\tests\DMValidator\CLI\
dotnet bin\Debug\netcoreapp2.1\DMConsoleValidator.dll -C validator.config
</pre>

where validator.config has something like:

<pre>
{
    "connection-string" : "&lt;enter_iothub_owner_connection_string_here&gt;",
    "tests" : "..\\Common\\TestCases\\TimeInfo\\",
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