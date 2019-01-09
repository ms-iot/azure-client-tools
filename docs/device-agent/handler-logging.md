# Handler Logging

The device agent comes with logging helper classes included in the utilities static library.

The logging helper classes support the following Targets:

- ETL.
    - On by default.
    - The provider guid is `e1688237-74f7-54f0-7ff7-9ba255fa157c`.
- Console
    - Off by default.
    - To turn on, call `Logger::EnableConsole(bool enable)`.
- Text File
    - Off by default.
    - To turn on, call `Logger::SetLogFilePath(const std::wstring& logFilePath, const std::wstring& prefix)`.

There should be one log file per binary (the logger is a singleton per binary) - and hence its configuration is per binary.

The device agent passes its own `textLogFilePath` configuration to the handlers it starts (in a call to `Start()`). 
It is recommended that the handler configures its own log location according to what's being passed in.

Related Topics:

- [Handler Start-up Configuration](handler-start-up-configuration.md)
- [Debugging](development-scenario/debugging.md)

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)