// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "AgentStub.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginHelpers.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\CrossBinaryRequest.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    shared_ptr<IMdmServer> AgentStub::_iMdmServer;
    shared_ptr<IRawHandlerHost> AgentStub::_iPluginHost;

    void AgentStub::SetMdmServer(std::shared_ptr<IMdmServer> iMdmServer)
    {
        _iMdmServer = iMdmServer;
    }

    void AgentStub::SetHandlerHost(std::shared_ptr<IRawHandlerHost> iPluginHost)
    {
        _iPluginHost = iPluginHost;
    }

    Json::Value AgentStub::InvokeRunSyncML(const Json::Value& parameters)
    {
        // Parameters
        string sid = parameters[JsonRawRunSyncMLSid].asString();
        string inSyncML = Base64ToString(parameters[JsonRawRunSyncMLInput].asString());

        // Invoke
        string outSyncML = _iMdmServer->RunSyncML(sid, inSyncML);

        // Output
        string outSyncMLBase64 = StringToBase64(outSyncML);
        Json::Value outSnycMLJson(Json::ValueType::objectValue);
        outSnycMLJson[JsonRawRunSyncMLOutput] = Json::Value(outSyncMLBase64);

        return outSnycMLJson;
    }

    Json::Value AgentStub::InvokeReport(const Json::Value& parameters)
    {
        // Parameters
        string id = parameters[JsonRawReportId].asString();
        DeploymentStatus deploymentStatus = DeploymentStatusFromString(parameters[JsonRawReportDeploymentStatus].asString());
        Json::Value value = parameters[JsonRawReportValue];

        // Invoke
        _iPluginHost->Report(id, deploymentStatus, value);

        // Output
        return Json::Value();
    }

    //
    // ReverseInvoke()
    //  Serializes/Deserializes cross-binary objects and exceptions.
    //
    // Inputs
    //   jsonInputString : json string of type CrossBinaryRequest.
    //
    // Returns:
    //   json string of type CrossBinaryResponse.
    //
    string AgentStub::ReverseInvoke(const std::string& jsonInputString) noexcept
    {
        std::string resultString;

        try
        {
            // Deserialize the string into a request.
            CrossBinaryRequest request = CrossBinaryRequest::FromJsonString(jsonInputString);

            // ReverseInvoke
            Json::Value output = ReverseInvoke(request.targetType, request.targetId, request.targetMethod, request.targetParameters);

            // Serialize the result into a response.
            resultString = CrossBinaryResponse::CreateFromSuccess(output).ToJson().toStyledString();
        }
        catch (const DMException& ex)
        {
            resultString = CrossBinaryResponse::CreateFromException(ex).ToJson().toStyledString();
        }
        catch (const exception& ex)
        {
            resultString = CrossBinaryResponse::CreateFromException(ex).ToJson().toStyledString();
        }
        catch (...)
        {
            resultString = CrossBinaryResponse::CreateFromException().ToJson().toStyledString();
        }

        return resultString;
    }

    //
    // ReverseInvoke()
    //  Routes the data to its destination.
    //
    // Inputs
    //   targetType       : host, raw handler, etc.
    //   targetId         : raw handler id, etc.
    //   targetMethod     : target method name.
    //   targetParameters : parameters that can be unpacked by the target method.
    //
    // Returns:
    //   Json object returned by targetMethod.
    //
    Json::Value AgentStub::ReverseInvoke(
        const std::string& targetType,
        const std::string& targetId,
        const std::string& targetMethod,
        const Json::Value& targetParameters)
    {
        (void)targetId;

        Json::Value output;

        if (targetType == JsonTargetTypeMdmServer)
        {
            if (targetMethod == JsonRawRunSyncML)
            {
                output = InvokeRunSyncML(targetParameters);
            }
            else
            {
                throw DMException(PLUGIN_ERR_INVALID_MDM_SERVER_OPERATION, "Invalid MDM server operation.");
            }
        }
        else if (targetType == JsonTargetTypePluginHost)
        {
            if (targetMethod == JsonRawReport)
            {
                output = InvokeReport(targetParameters);
            }
            else
            {
                throw DMException(PLUGIN_ERR_INVALID_RAW_HANDLER_HOST_OPERATION, "Invalid raw handler host operation.");
            }
        }
        else
        {
            throw DMException(PLUGIN_ERR_INVALID_PLUGIN_HOST_TARGETTYPE, "Invalid plugin host target type.");
        }
        return output;
    }

    //
    // ReverseInvoke()
    //
    // Inputs
    //   jsonInputString : CrossBinaryRequest serialized into a json string.
    //
    // Outpus:
    //   jsonOutputString : CrossBinaryResponse serialized into a json string.
    //
    int __stdcall AgentStub::ReverseInvoke(const char* jsonInputString, char** jsonOutputString)
    {
        if (jsonInputString == NULL || jsonOutputString == NULL)
        {
            return PLUGIN_ERR_INVALID_ARG;
        }

        string resultString = ReverseInvoke(jsonInputString);

        *jsonOutputString = new char[resultString.size() + 1];
        memcpy(*jsonOutputString, resultString.c_str(), resultString.size() + 1);

        return PLUGIN_ERR_SUCCESS;
    }

    int __stdcall AgentStub::ReverseDeleteBuffer(const char* buffer)
    {
        delete[] buffer;
        return PLUGIN_ERR_SUCCESS;
    }

}}}}
