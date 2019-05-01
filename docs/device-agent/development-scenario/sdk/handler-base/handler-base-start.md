# HandlerBase::Start()

### Signature
<pre>
virtual void Start(
    const Json::Value& config,
    bool& active);
</pre>

### Purpose
- The agent notifies the handler it can start executing its code. This may involve starting a back ground thread or starting a service, for example.
- The agent passes the handler its static configuration (log file, or anything the handler defines and expects the device builder to place in the agent's configuration file under this handler's section.)
- The handler indicates whether it is ready to receive cloud communication or not.

### Timing
- called after loading the plug-in, and before the cloud connection is ready.
- The handler should not try to send anything to the cloud in Start(). It should wait for the connection status change notification.

### Parameters

| Name | Type | In/Out | Description |
|------|------|--------|-------------|
| config | Json::Value | In | |
| active | bool | Out | |

### Sample Override

<pre>
    void DeviceInfoHandler::Start(
        const Json::Value& handlerConfig,
        bool& active)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        SetConfig(handlerConfig);

        // Text file logging...
        Json::Value logFilesPath = handlerConfig[JsonTextLogFilesPath];
        if (!logFilesPath.isNull() && logFilesPath.isString())
        {
            wstring wideLogFileName = MultibyteToWide(logFilesPath.asString().c_str());
            wstring wideLogFileNamePrefix = MultibyteToWide(DeviceInfoHandlerId);
            gLogger.SetLogFilePath(wideLogFileName.c_str(), wideLogFileNamePrefix.c_str());
            gLogger.EnableConsole(true);

            TRACELINE(LoggingLevel::Verbose, "Logging configured.");
        }

        active = true;
    }
</pre>

----

[Development Scenario Walk-Through](../../../development-scenario.md) | [Authoring New Plugins](../../developer-plugin-creation.md) | [HandlerBase](handler-base.md)
