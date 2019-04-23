# Servicing

## Overview 

The device agent and its plug-ins are provided as open source. The OEM is expected to build, sign, and create Windows packages for those binaries.

Those packages are then made part of the image or deployed to the devices in the field. The exact mechanism differs, however, between Windows IoT Core and Windows IoT Enterprise.

## Servicing on Windows IoT Core

On Windows IoT Core, those packages are included in the image that is to be flashed to the devices on the factory floor.

For post production updates, the OEM will create a new package exactly like their own, upload it to Windows Update, and then target the intended devices using DUC (Device Update Portal).
For more information, see the [Device Update Center User Guide](https://docs.microsoft.com/en-us/windows-hardware/service/iot/using-device-update-center).

## Servicing on Windows IoT Enterprise

On Windows IoT Enterprise, those packages can be included in the image using DISM.

For post production updates, the OEM can use the same mechanism currently in use to deliver updates to their own applications to deliver these cabs to the device and deploy them using dism.

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)


