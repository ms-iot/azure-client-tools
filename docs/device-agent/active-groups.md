# Active Groups

Each handler is given the opportunity to indicate whether it is available at runtime or not. This is specially useful when implementing handlers for things like pluggable hardware.

The device agent queries each handler at start-up (`Start()`) to check whether it is active or not. If not, subsequent attempts to modify the target twin configuration group will be ignored.

- ToDo: A handler needs a mechanism to signal its state changes.
- ToDo: The handler schema state needs to be reflected in the twin device schemas.
- ToDo: If a twin change targets an inactive group, an error should be reported.

Related Topics

- [Extensibility and Discovery Model](extensibility-and-discovery-model.md)

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)