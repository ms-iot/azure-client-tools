// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define EnumerateLogFoldersCmdHandlerId "EnumerateLogFoldersCmd"
#define EnumerateLogFilesCmdHandlerId "EnumerateLogFilesCmd"
#define DiagnosticLogsManagementStateHandlerId "eventTracingCollectors"
#define DeleteLogFileCmdHandlerId "DeleteLogFileCmd"
#define UploadLogFileCmdHandlerId "UploadLogFileCmd"

#define PayloadList "list"
#define PayloadFolderName "folderName"
#define PayloadFileName "fileName"
#define PayloadConnectionString "connectionString"
#define PayloadContainer "container"

#define JsonNoString "no"
#define JsonYesString "yes"
#define JsonReportLevelMinimal "minimal"
#define JsonReportLevelDetailed "detailed"
#define JsonReportLevelNone "none"
#define JsonReportProperties "reportProperties"

//collector
#define JsonFileModeSequential "sequential"
#define JsonFileModeCircular "circular"
#define JsonCollectorTraceLogFileMode "traceLogFileMode"
#define JsonCollectorLogFileSizeLimitMB "logFileSizeLimitMB"
#define JsonCollectorLogFileFolder "logFileFolder"
#define JsonCollectorLogFileName "logFileName"
#define JsonCollectorStarted "started"
#define JsonProviders "providers"

//provider
#define JsonProviderTraceLevel "traceLevel"
#define JsonProviderKeywords "keywords"
#define JsonProviderEnabled "enabled"
#define JsonProviderType "type"
#define JsonTraceLevelCritical "critical"
#define JsonTraceLevelError "error"
#define JsonTraceLevelWarning "warning"
#define JsonTraceLevelInformation "information"
#define JsonTraceLevelVerbose "verbose"