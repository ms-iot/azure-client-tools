# Correlating Desired and Reported Properties

The IoT Hub twin service does not define a correlation between the desired and reported properties. This correlation is left to the implementation.

The device agent implementation enforces the correlation only at the configuration group level through its identity.

The same configuration group identifier in the desired properties section is also used to identify the reported state in the reported section.

The handler author is responsible for defining how properties in the desired and reported sections under that identifier correlate.

The general convention is that all desired properties are also reported. In addition to those, the handler may choose to report additional state.
State that is only reported, but cannot be configured, can be thought of as 'read-only' state.

For example, the `rebootInfo` handler defines the following:

<pre>
{
    "properties": {
        "desired": {
            "rebootInfo": {
                "dailyReboot": "",
                "oneTimeReboot": ""
            }
        },
        "reported": {
            "rebootInfo": {
                "dailyReboot": "",
                "oneTimeReboot": "",
                "lastRebootTime": ""
            }
        }
    }
}
</pre>

In the example above, it is implied that "reported.rebootInfo.dailyReboot" is the current state of what "desired.rebootInfo.dailyReboot" sets.
Note that "reported.rebootInfo.lastRebootTime" appears only in reported, which indicates that there is no way to configure it (read-only property).

In some cases, the author of the handler may choose to not report some of the desired properties. An example of that is where the state is still represented through a different property.

**Note**:

One practice that must be observed is that when reporting a property, the implementation should always query its value from the underlying component - i.e. get the current actual state. It should not just used the desired property value that's been passed and report it. Such value does not tell the operator the current state.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)