// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// Plug-in Constants

// Request
#define JsonTargetType "type"
#define JsonTargetTypeHost "host"
#define JsonTargetTypeRaw "raw"
#define JsonTargetTypePluginHost "pluginHost"
#define JsonTargetTypeMdmServer "mdmServer"

#define JsonTargetId "id"
#define JsonTargetMethod "method"
#define JsonTargetParameters "parameters"

// Plugin ----

// Plugin - GetInterfaceIds
#define JsonHostGetHandlersInfo "getHandlersInfo"
#define JsonHostGetHandlersInfoResult "result"
#define JsonHostGetHandlersInfoResultId "id"
#define JsonHostGetHandlersInfoResultType "type"

// Plugin - CreateRawHandler
#define JsonHostCreateRawHandler "createRawHandler"
#define JsonHostCreateRawHandlerId "id"
#define JsonHostCreateRawHandlerResult "result"

// Plugin - DestroyRawHandler
#define JsonHostDestroyRawHandler "destroyRawHandler"
#define JsonHostDestroyRawHandlerId "id"
#define JsonHostDestroyRawHandlerResult "result"

// Raw Handler ----

// Handler - GetHandlerType
#define JsonGetHandlerType "handlerType"
#define JsonGetHandlerTypeResult "result"

// Raw Handler - Start
#define JsonRawStart "start"
#define JsonRawStartResult "result"

// Raw Handler - Stop
#define JsonRawStop "stop"

// Raw Handler - OnConnectionStatusChanged
#define JsonRawOnConnectionStatusChanged "onConnectionStatusChanged"
#define JsonConnectionStatusValue "value"
#define JsonConnectionStatusOffline "offline"
#define JsonConnectionStatusOnline "online"

// Raw Handler - IsConfigured
#define JsonRawIsConfigured "isConfigured"
#define JsonRawIsConfiguredResult "result"

// Raw Handler - GetDeploymentStatus
#define JsonRawGetDeploymentStatus "getDeploymentStatus"
#define JsonRawGetDeploymentStatusResult "result"

// Raw Handler - SetDeploymentStatus
#define JsonRawSetDeploymentStatus "setDeploymentStatus"
#define JsonRawSetDeploymentStatusValue "value"
#define JsonRawSetDeploymentStatusSucceeded "succeeded"
#define JsonRawSetDeploymentStatusNotStarted "notStarted"
#define JsonRawSetDeploymentStatusPending "pending"
#define JsonRawSetDeploymentStatusFailed "failed"

// Raw Handler - InvokeResult
#define JsonInvokeResultPresent "present"
#define JsonInvokeResultCode "code"
#define JsonInvokeResultPayload "payload"

// Raw Handler - Invoke
#define JsonRawInvoke "invoke"

// Raw Handler - GetDeploymentStatusJson
#define JsonRawGetDeploymentStatusJson "getDeploymentStatusJson"
#define JsonRawGetDeploymentStatusJsonResult "result" 

// Raw Handler - GetReportedSchema
#define JsonRawGetReportedSchema "getReportedSchema"
#define JsonRawGetReportedSchemaResult "result"

// Plugin Host ----

// Plugin Host - Report
#define JsonRawReport "report"
#define JsonRawReportId "id"
#define JsonRawReportDeploymentStatus "deploymentStatus"
#define JsonRawReportValue "value"

// MdmServer ----

// MdmServer - RunSyncML
#define JsonRawRunSyncML "runSyncML"
#define JsonRawRunSyncMLSid "sid"
#define JsonRawRunSyncMLInput "inputSyncML"
#define JsonRawRunSyncMLOutput "outputSyncML"

