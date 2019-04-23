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
#define JsonHostCreateHandlerId "id"

// Plugin - DestroyRawHandler
#define JsonHostDestroyRawHandler "destroyRawHandler"
#define JsonHostDestroyHandlerId "id"

// Raw Handler ----

// Handler - GetHandlerType
#define JsonGetHandlerType "handlerType"
#define JsonGetHandlerTypeResult "result"

// Raw Handler - Start
#define JsonHandlerStart "start"
#define JsonHandlerStartResult "result"

// Raw Handler - Stop
#define JsonHandlerStop "stop"

// Raw Handler - OnConnectionStatusChanged
#define JsonHandlerOnConnectionStatusChanged "onConnectionStatusChanged"
#define JsonHandlerConnectionStatusValue "value"
#define JsonHandlerConnectionStatusOffline "offline"
#define JsonHandlerConnectionStatusOnline "online"

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

// Handler - SendEvent
#define JsonRawSendEvent "sendEvent"
#define JsonRawSendEventInterfaceName "interfaceName"
#define JsonRawSendEventEventName "eventName"
#define JsonRawSendEventMessageData "messageData"

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

