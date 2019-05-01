# Plug-in Manifest File

## Purpose

The Plug-in Manifest File declares information about a specific plug-in; for example the executable file name, entry point type, etc.
## Sample

<pre>
{
  "codeFileName": "ComponentNamePlugin.dll",
  "direct": true|false,
  "outOfProc": true|false,
  "keepAliveTime": number,
  "handlers": [
    {
      "id": "camera",
      "dependencyList": ["wifihandler"]
    }
  ]
}
</pre>

## Fields

| Name | Type | Description |
|-----|-----|-----|
| `codeFileName` | string | absolute or relative file name of the file containing the plug-in implementation. |
| `direct` | boolean | whether this binary supports direct calls into the binary (`true`) or marshalling is required (`false`). |
| `outOfProc` | boolean | whether this binary should be loaded in a separate process or within in the agent process.<br/><br/>This parameter is applicable only if `direct` is set to `false` (i.e. marshalling has to take place). |
| `keepAliveTime` | unsigned integer | the maximum time in milliseconds after which the plug-in host process will be terminated if it there is no activity.<br/><br/>This parameter is applicable only if `outOfProc` is set to `true`.<br/><br/>If the plug-in is expected to continue running without activity, set this value to 0xFFFFFFFF (can be useful for handlers that send telemetry).|
| `handlers` | see Handlers below. |


### Handlers

The 'handlers' field is an Array that allows the specification handler-specific properties:

| Name | Value |
|-----|-----|
| `id` | the name of the handler |
| `dependencyList` | see dependency list below. |

#### Dependency List

The 'dependencyList' is a list of handler dependencies that this handler is dependent on. These dependencies will be executed before the said handler when properties are changed through the device twin.

The dependency list is represented bellow:
<pre>
{
  "dependencyList": ["handler1, "handler2"]
}
</pre>

## Location and Naming

The manifest file must be placed in the manifests folder where the Device Agent will look for it during the plug-in enumeration phase.

The file must have the .json extension.

----

[Reference](../reference.md)
