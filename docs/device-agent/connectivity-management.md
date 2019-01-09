# Connectivity Management

## Overview

The device agent maintains the connectivity to Azure IoT Hub - this includes, initial connection, connection renewal, and connection restoration.

## Intial Connection and Connection Renewal

Once device agent is started, it goes through the following sequence:

<pre>
- Retrieve the connection string SAS token from the designated TPM slot.
    - If not found, invoke the DPS client library to retrieve it.
- Establish the connection to IoT Hub.
- Signal Start() to all handlers.
- Do
    - Process any pending work on the IoT Hub connection.
    - Sleep for 10 milliseconds.
    - Check if the agent has received a signal to exit.
    - Check if it is within the grace period to renew the connection, if yes
        - Signal Stop() to all handlers.
        - Deinitialize the Azure SDK objects.
        - Re-establish the connection to IoT Hub.
        - Re-signal Start() to all handlers.
</pre>

To configure the durations used in the logic described above, the [Device Agent Configuration File](reference/device-agent-configuration-file.md) defines the following parameters:

| Name | Type | Required | Units | Description |
|----|----|----|----|----|
| `connectionStringExpiry` | integer | Yes| seconds | This value is used to generate the SAS token from the TPM. It indicates how long the generate SAS token is valid for in seconds. |
| `sasExpiryInterval` | integer | Yes| seconds | This indicates the maximum time before the SAS token has to be renewed in seconds. |
| `sasRenewalGracePeriod` | integer | Yes| seconds | This indicates when the SAS token should be renewed before reaching the sasExpiryInterval.<br/><br/>The device agent will attempt to renew the SAS token the next time it is within 'sasRenewalGracePeriod' seconds from the 'sasExpiryInterval'.|

Sample:

<pre>
{
    "connectionStringExpiry": 3600,
    "sasExpiryInterval" : 86400,
    "sasRenewalGracePeriod": 3600
}
</pre>

## Connection Restoration

ToDoc

----

[Home](../../README.md) | [Device Agent](device-agent.md) | [Reference](reference.md)