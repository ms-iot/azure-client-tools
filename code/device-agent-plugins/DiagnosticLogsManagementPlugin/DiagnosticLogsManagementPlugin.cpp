// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "device-agent/common/plugins/PluginJsonConstants.h"
#include "device-agent/plugin-common/PluginStub.h"
#include "PluginJsonConstants.h"
#include "EnumerateLogFoldersCmdHandler.h"
#include "EnumerateLogFilesCmdHandler.h"
#include "DeleteLogFileCmdHandler.h"
#include "UploadLogFileCmdHandler.h"
#include "DiagnosticLogsManagementStateHandler.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

using namespace Microsoft::Azure::DeviceManagement::Plugin::Common;
using namespace Microsoft::Azure::DeviceManagement::DiagnosticLogsManagementPlugin;

PluginStub gPluginStub;
std::shared_ptr<IRawHandler> gEnumerateLogFoldersHandler;
std::shared_ptr<IRawHandler> gEnumerateLogFilesHandler;
std::shared_ptr<IRawHandler> gDeleteLogFileHandler;
std::shared_ptr<IRawHandler> gUploadLogFileHandler;
std::shared_ptr<IRawHandler> gDiagnosticLogsManagementStateHandler;

std::shared_ptr<IRawHandler> CreateEnumerateLogFoldersHandler()
{
    if (gEnumerateLogFoldersHandler == nullptr)
    {
        gEnumerateLogFoldersHandler = make_shared<EnumerateLogFolders>();
    }
    return gEnumerateLogFoldersHandler;
}

std::shared_ptr<IRawHandler> CreateEnumerateLogFilesHandler()
{
    if (gEnumerateLogFilesHandler == nullptr)
    {
        gEnumerateLogFilesHandler = make_shared<EnumerateLogFiles>();
    }
    return gEnumerateLogFilesHandler;
}

std::shared_ptr<IRawHandler> CreateDeleteLogFileHandler()
{
    if (gDeleteLogFileHandler == nullptr)
    {
        gDeleteLogFileHandler = make_shared<DeleteLogFile>();
    }
    return gDeleteLogFileHandler;
}

std::shared_ptr<IRawHandler> CreateUploadLogFileHandler()
{
    if (gUploadLogFileHandler == nullptr)
    {
        gUploadLogFileHandler = make_shared<UploadLogFile>();
    }
    return gUploadLogFileHandler;
}

std::shared_ptr<IRawHandler> CreateDiagnosticLogsManagementStateHandler()
{
    if (gDiagnosticLogsManagementStateHandler == nullptr)
    {
        gDiagnosticLogsManagementStateHandler = make_shared<DiagnosticLogsManagementStateHandler>();
    }
    return gDiagnosticLogsManagementStateHandler;
}

int __stdcall PluginCreate()
{
    gPluginStub.Initialize();
    gPluginStub.RegisterRawHandler(EnumerateLogFoldersCmdHandlerId, CreateEnumerateLogFoldersHandler);
    gPluginStub.RegisterRawHandler(EnumerateLogFilesCmdHandlerId, CreateEnumerateLogFilesHandler);
    gPluginStub.RegisterRawHandler(DeleteLogFileCmdHandlerId, CreateDeleteLogFileHandler);
    gPluginStub.RegisterRawHandler(UploadLogFileCmdHandlerId, CreateUploadLogFileHandler);
    gPluginStub.RegisterRawHandler(DiagnosticLogsManagementStateHandlerId, CreateDiagnosticLogsManagementStateHandler);
    return DM_ERROR_SUCCESS;
}

#include "device-agent/plugin-common/PluginExports.h"