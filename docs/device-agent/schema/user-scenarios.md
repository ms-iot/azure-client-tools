# 1. User Scenarios
[Schema](../schema.md)

The target user is the operator. The operator is tasked with monitoring thousands of devices in the field. The schema described in this document is optimized to serve the operators scenarios in an efficient way (in terms of network traffic and process power).

The operator is expected to:

1. Determine the current desired configurations of any given device and where they come from (which deployments).
2. Manage (configure, and monitor) 300~500 configuration properties per device - in groups or individually.
3. Manage subsets of devices based on certain criteria.
4. Monitor the progress of new configurations as they are being deployed; specifically: 
    1. Progress 
    2. Errors 
    3. Compliance 
5. Monitor the health of the device and remediate errors:
   1. Find devices with errors.
   2. Understand the errors and their root cause.
   3. Create and deploy the right fix.
6. Easily and efficiently migrate the configuration from one version to the next. 
7. The portal needs to work with various client versions.
8. The device needs to be capable of interacting using multiple interfaces (NativeJson, etc).

----

Last Updated 9/6/2018

[Schema](../schema.md)