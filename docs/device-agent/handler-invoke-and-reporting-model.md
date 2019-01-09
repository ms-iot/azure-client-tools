# Handler Invoke and Reporting Model

The device agent supports two mechanisms for changing the state on a device:

- Twin updates
- Direct method calls

When designing management functionality, the decision to choose one or the other relies on the semantics of the desired change.

## Choosing Twin Updates vs. Direct Method Calls

### Twin Updates

If the goal is a specific final state for the device, then, relying on the IoT Hub twin provides a very good support for communicating with the device that final state.

For example, if the device is offline at the time the final state is configured in the twin, when the device comes online it will still be able to get notified of the new desired state and it is up to the device agent to get the device to that state.

Another scenario is when the operator needs to keep track of what state they intended their devices to be, then the twin provides a way to store and query that information. This can be very useful in cases where the operator wants to know if there are are discrepencies between what is intended (desired) and the current device state (reported).

### Direct Method Calls

If the goal is to execute certain functionality on the device without keeping track of the desired state, and it is a one time event, a direct method is a good fit for this scenario.

For example, if an operator wants to reboot the device, this is an action - not a final desired state. A direct method would be the right thing to use here. 
Note that the direct method handler on the device can still report back through either the direct method return value or through the twin reported properties.

## Invocation Semantics

In both mechanisms mentioned above, note that the fundamental difference is in how intent is stored. In one (twin updates), the intent is stored in the twin - while the other it is not stored any where in the cloud (unless there's some custom service built to track them, of course').

But looking at the receiving end, i.e. the device, both look like an operation with some parameters. For example:

<pre>
{
    "desired": {
        "rebootInfo": {
            "dailyReboot": "some_time"
        }
    }
}
</pre>

The above can be interpreted on the device as: "Set rebootInfo.dailyReboot to some_time" - which is simply a method call.

Because of this similarity to method calls, handlers for both twin changes and direct methods share the same interface: `Invoke()`.

The difference is that in the case of twin change, no return payload is expected, while in the case of a direct method, a return payload is expected.

#### InvokeResult

The returned state and payload is expessed in the `InvokeResult` return data type by setting `present` to `true` or `false`.
- `true` means the payload is present and must be set as such for direct method handlers.
- `false` means the payload is not present and must be set as such for twin change handlers.

ToDo: need to retype/rename `present` to `context`.

## Reporting

There are two channels for the device agent to communicate the resulting state to the cloud:

- The twin reported properties.
    - This channel is available for both twin update operations as well as direct method calls.
        - For direct method calls, this is specially useful for asynchronous calls where they have to return and not wait for the final state. When the final state is ready, it can then be reported to the twin.
    - This channel can be invoked any number of time while `Invoke()` is still executing.
    - This is done through the `ReportAndFinalize` method.
    - Note that the author of the handler should honor `reportingMode` of the [meta data object](schema/configuration-groups/meta-object.md) for their handler.


- The direct method return code and payload.
    - This channel is available only for direct method calls.
        - This is the synchronous return result of the direct method. If the method is asynchronous, this return can be as simple as "yes, I've received the request'". The final result can then be reported later to the twin.
    - This channel can be used only once per `Invoke()`.
    - This is done through the `InvokeResult` type and setting `present` to `true`.



Related Topics

- [Meta Data Object](schema/configuration-groups/meta-object.md)

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)