# Deployment States

When deploying a new configuration to a device pool, each device will receive the new configuration at a certain point, queue it internally, and then start processing the new configuration.
When processing is complete, the device should report whether it was successful or not.

If a device receives new configuration, then its reported state for that configuration at that time is transient.
Instead, the observer (the solution) should wait until it is either successful or not.
Also, monitoring this process can provide insights on timing and maybe analytics.

This is represented in the current device agent implementation by the deployment status.
When a configuration group is received, the device agent marks the deployment status of that group as `pending`. This is done for all the groups included in an IoT Hub notification.
When the handler of a particular configuration group is done processing it, that handler is responsible for reporting the final deployment state (failure or success) to the twin.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)