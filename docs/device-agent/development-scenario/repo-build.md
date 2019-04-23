# Repo Build

## First Time Build

Assuming that the target device/OS is an x64 one, the following steps will build all the necessary bits for that architecture:

- Open the VS command prompt and run:

  <pre>
  cd / d:\git\dm\scripts
  build.cmd x64 Debug 10.0.16299.0
  </pre>

Note: If the Start menu does not show the VS command prompt, try:
<pre>
%comspec% /k "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
</pre>

The generated files will always be under:
<pre>
&lt;repo_root&gt;\code\output\&lt;architecture&gt\&lt;flavor&gt\
</pre>

Where
- &lt;architecture&gt; can be: `arm`, `x64`, or `win32`.
- &lt;flavor&gt; can be: `debug`, or `release`.

## Building Code Changes

Depending on the scenario...

- For coding and fast iteration on building/testing, open the `code\AzureDeviceManagement.sln` in Visual Studio and build the flavor/architecture you need.
- For verification before check-in, run `scripts\build.azure-dm.check-in.cmd` from the Visual Studio command prompt. This will build all architectures and all flavors.
- For re-building after pulling a new version of the Azure C-SDK, run `scripts\build.azure-c-sdk.check-in.cmd` from a Visual Studio command prompt.

----

[Development Scenario](../development-scenario.md)
