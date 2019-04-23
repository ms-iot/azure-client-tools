// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>

// Plug-in Manifest Constants
#define JsonPluginCodeFileName "codeFileName"
#define JsonPluginDirect "direct"
#define JsonPluginOutOfProc "outOfProc"
#define JsonPluginKeepAliveTime "keepAliveTime"
#define JsonPluginHandlers "handlers"

#define JsonPluginHandlerId "id"
#define JsonPluginHandlerDependencyList "dependencyList"

// Service Parameters
#define JsonTextLogFilesPath "textLogFilesPath"
#define JsonPluginManifestsPath "pluginManifestsPath"
#define JsonHandlerTestMode "handlerTestMode"
#define JsonPluginsDataPath "pluginsDataPath"
#define JsonSasExpiryInterval "sasExpiryInterval"
#define JsonSasRenewalGracePeriod "sasRenewalGracePeriod"
#define JsonDpsIsEnabled "dpsEnabled"
#define JsonDpsUri "dpsUri"
#define JsonDpsScopeId "dpsScopeId"
#define JsonDeviceSlotNumber "deviceSlotNumber"
#define JsonConnectionString "connectionString"
#define JsonDmModuleId "dmModuleId"

#define JsonAzureInterfaceType "azureInterfaceType"
#define JsonAzureInterfaceTypeRaw "raw"

#define JsonDmModuleSlotNumber "dmModuleSlotNumber"
#define JsonOtherModuleIds "otherModuleIds"
#define JsonHandlerConfigRoot "handlers"
#define JsonHandlerConfigReportOnConnect "reportOnConnect"

// Handler Types
#define JsonHandlerTypeRaw "handlerTypeRaw"

// Error object
#define JsonErrorsSectionName "errors"
#define JsonNonSectionErrors "nonSectionErrors"

// Cross-Binary Error object
#define JsonCrossBinaryResponseSuccess "success"
#define JsonCrossBinaryResponsePayload "payload"
#define JsonCrossBinaryErrorType "errorType"
#define JsonCrossBinaryErrorTypeDMException "DMException"
#define JsonCrossBinaryErrorTypeStdException "StdException"
#define JsonCrossBinaryErrorTypeUnknown "unknown"
#define JsonCrossBinaryErrorSubSystem "errorSubSystem"
#define JsonCrossBinaryErrorCode "errorCode"
#define JsonCrossBinaryErrorMessage "errorMessage"
#define JsonCrossBinaryErrorMessageUnknown "Unknown exception."

#define JsonReported "reported"
#define JsonDesired "desired"
#define JsonWindows "windows"

#define JsonClearReportedCmd "clearReportedCmd"

#define JsonDirectMethodEmptyPayload "{}"
#define JsonDirectMethodSuccessCode 200
#define JsonDirectMethodFailureCode 400

#define JsonMethodNotFoundCode -2
#define JsonMethodNotFoundMessage "Method not found"

// Common Handler Constants
#define JsonIgnore "<ignore>"
#define JsonRefreshing "Refreshing"

#define JsonTime "time"

// Device Schemas
#define JsonDeviceSchemas "__deviceSchemas"
#define JsonDeviceSchemasReporting "reporting"
#define JsonDeviceSchemasGroups "groups"
#define JsonDeviceSchemasTypes "types"
#define JsonDeviceSchemasTags "tags"
#define JsonDeviceSchemasAll "all"
#define JsonDeviceSchemasConfigured "configured"
#define JsonDeviceSchemasTypeRaw "raw"

#define JsonDeviceSchemasTagDM "dm"
#define JsonDeviceSchemasTagSensor "sensor"

#define JsonDeviceSchemaType "type"
#define JsonDeviceSchemaMaxInputVersion "maxInputVersion"
#define JsonDeviceSchemaOutputVersion "outputVersion"

// Reported Summary Object
#define JsonReportedSummary "__summary"
#define JsonFailedCount "failedCount"
#define JsonFailedGroups "failedGroups"
#define JsonPendingCount "pendingCount"
#define JsonPendingGroups "pendingGroups"
#define JsonDeployments "deployments"

// Schema Object
#define JsonVersion "version"
#define JsonMaxDesiredVersion "maxDesiredVersion"
#define JsonMaxReportedVersion "maxReportedVersion"

// Meta Object
#define JsonMeta "__meta"

#define JsonDeploymentId "deploymentId"
#define JsonDeploymentIdUnspecified "unspecified"
#define JsonDeploymentIdDefault JsonDeploymentIdUnspecified

#define JsonDependencies "dependencies"
#define JsonDependenciesDefault ""

#define JsonReportingMode "reportingMode"
#define JsonReportingModeNone "none"
#define JsonReportingModeDetailed "detailed"
#define JsonReportingModeMinimal "minimal"
#define JsonReportingModeDefault JsonReportingModeDetailed

#define JsonDeploymentStatus "deploymentStatus"
#define JsonDeploymentStatusNotStarted "notStarted"
#define JsonDeploymentStatusSucceeded "succeeded"
#define JsonDeploymentStatusPending "pending"
#define JsonDeploymentStatusFailed "failed"

// Reported Error Object
#define JsonErrorSubsystem "subsystem"
#define JsonErrorSubsystemUnknown "unknown"
#define JsonErrorSubsystemPlatform "platform"
#define JsonErrorContext "context"
#define JsonErrorCode "code"
#define JsonErrorMessage "message"
#define JsonErrorParameters "parameters"

// Generic Errors
#define JsonErrorGenericCode -1
#define JsonErrorJsonParseError -2

// Reported Error List Object
#define JsonErrorList "__errors"

// Reported Error Object
#define JsonErrorSubsystemDeviceManagement "deviceManagement"
