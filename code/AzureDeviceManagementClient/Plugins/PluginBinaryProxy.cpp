// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginBinaryProxy.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginHelpers.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\CrossBinaryRequest.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "AgentStub.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    PluginBinaryProxy::PluginBinaryProxy(const std::string& pluginFullPath) :
        _hModule(NULL),
        _pluginFullPath(pluginFullPath),
        PluginCreate(NULL),
        PluginInvoke(NULL),
        PluginDeleteBuffer(NULL),
        PluginSetReverseInvoke(NULL),
        PluginSetReverseDeleteBuffer(NULL)
    {}

    void PluginBinaryProxy::Load()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        wstring win32FullFilePath = Utils::MultibyteToWide(_pluginFullPath.c_str());
        _hModule = LoadLibrary(win32FullFilePath.c_str());
        if (_hModule == NULL)
        {
            string msg = "Failed to load library: " + _pluginFullPath;
            throw DMException(GetLastError(), msg);
        }
        TRACELINEP(LoggingLevel::Verbose, "Successfully loaded: ", _pluginFullPath.c_str());

        // PluginCreateStr
        PluginCreate = (PluginCreatePtr)GetProcAddress(_hModule, PluginCreateStr);
        if (NULL == PluginCreate)
        {
            string msg = string("Failed to find entry point: ") + PluginCreateStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(PLUGIN_ERR_MISSING_ENTRY_POINT, msg);
        }

        // PluginInvokeStr
        PluginInvoke = (PluginInvokePtr)GetProcAddress(_hModule, PluginInvokeStr);
        if (NULL == PluginInvoke)
        {
            string msg = string("Failed to find entry point: ") + PluginInvokeStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(PLUGIN_ERR_MISSING_ENTRY_POINT, msg);
        }

        // PluginDeleteStr
        PluginDeleteBuffer = (PluginDeleteBufferPtr)GetProcAddress(_hModule, PluginDeleteBufferStr);
        if (NULL == PluginDeleteBuffer)
        {
            string msg = string("Failed to find entry point: ") + PluginDeleteBufferStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(PLUGIN_ERR_MISSING_ENTRY_POINT, msg);
        }

        // PluginSetReverseInvokeStr
        PluginSetReverseInvoke = (PluginSetReverseInvokePtr)GetProcAddress(_hModule, PluginSetReverseInvokeStr);
        if (NULL == PluginSetReverseInvoke)
        {
            string msg = string("Failed to find entry point: ") + PluginSetReverseInvokeStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(PLUGIN_ERR_MISSING_ENTRY_POINT, msg);
        }

        // PluginSetReverseDeleteBufferStr
        PluginSetReverseDeleteBuffer = (PluginSetReverseDeleteBufferPtr)GetProcAddress(_hModule, PluginSetReverseDeleteBufferStr);
        if (NULL == PluginSetReverseDeleteBuffer)
        {
            string msg = string("Failed to find entry point: ") + PluginSetReverseDeleteBufferStr;
            TRACELINE(LoggingLevel::Error, msg.c_str());
            throw DMException(PLUGIN_ERR_MISSING_ENTRY_POINT, msg);
        }

        // Initialize...
        PluginCreate();
        PluginSetReverseInvoke(AgentStub::ReverseInvoke);
        PluginSetReverseDeleteBuffer(AgentStub::ReverseDeleteBuffer);
    }

    void PluginBinaryProxy::Unload()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        if (_hModule != NULL)
        {
            FreeLibrary(_hModule);
        }

        // Set all the function pointers to null
        PluginCreate = NULL; 
        PluginInvoke = NULL;
        PluginDeleteBuffer = NULL;
        PluginSetReverseInvoke = NULL;
        PluginSetReverseDeleteBuffer = NULL;
    }

    Json::Value PluginBinaryProxy::Invoke(
        const std::string& targetType,
        const std::string& targetId,
        const std::string& targetMethod,
        const Json::Value& targetParameters) const
    {
        if (NULL == PluginInvoke)
        {
            throw DMException(PLUGIN_ERR_MISSING_ENTRY_POINT, "Failed to find entry point PluginInvoke.");
        }

        string request = CrossBinaryRequest::BuildRequest(targetType, targetId, targetMethod, targetParameters);

        char* responseBuffer;
        int retCode = PluginInvoke(request.c_str(), &responseBuffer);
        if (retCode != PLUGIN_ERR_SUCCESS)
        {
            throw DMException(retCode, "PluginInvoke returned error.");
        }

        if (*responseBuffer == NULL)
        {
            throw DMException(PLUGIN_ERR_INVALID_OUTPUT_BUFFER, "PluginInvoke returned invalid buffer.");
        }

        // Copy to local heap.
        string responseString = responseBuffer;

        // Delete from plug-in heap.
        PluginDeleteBuffer(responseBuffer);

        // Translates failures to exceptions.
        return CrossBinaryResponse::GetPayloadFromJsonString(responseString);
    }



}}}}
