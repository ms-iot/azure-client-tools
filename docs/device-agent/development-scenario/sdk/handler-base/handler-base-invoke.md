# HandlerBase::Invoke()

### Signature

<pre>
DMCommon::InvokeResult Invoke(
    const Json::Value& groupDesiredConfigJson) noexcept;
</pre>


### Purpose

This method is responsible for parsing the incoming json and translating it to system calls or configurations.

### Timing

This method is called once a connection to the cloud is established, and again everytime a change occurs in the twin in the section the handler is associated with.

### Parameters

### Sample Override

----

[Development Scenario Walk-Through](../../../development-scenario.md) | [Authoring New Plugins](../../developer-plugin-creation.md) | [HandlerBase](handler-base.md)
