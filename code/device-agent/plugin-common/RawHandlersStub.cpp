// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "stdafx.h"
#include "RawHandlersStub.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    RawHandlerStub::RawHandlerStub()
    {}

    void RawHandlerStub::SetAgentBinaryProxy(
        AgentBinaryProxy* agentBinaryProxy)
    {
        _agentBinaryProxy = agentBinaryProxy;
    }

    void RawHandlerStub::SetHandlerHostProxy(
        std::shared_ptr<RawHandlerHostProxy> handlerHostProxy)
    {
        _handlerHostProxy = handlerHostProxy;
    }

    void RawHandlerStub::RegisterHandler(
        const string& id,
        function<shared_ptr<IRawHandler>()> createFactory)
    {
        // ToDo: Handle duplicates by returning an error.
        if (_rawFactoryMap.cend() != _rawFactoryMap.find(id))
        {
            return;
        }
        _rawFactoryMap[id] = createFactory;
    }

    void RawHandlerStub::UnregisterHandler(
        const string& id)
    {
        _rawFactoryMap.erase(id);
    }

    Json::Value RawHandlerStub::CreateHandler(
        const Json::Value& config)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        string id = config[JsonHostCreateHandlerId].asString();

        RawHandlerFactoryMap::const_iterator it = _rawFactoryMap.find(id);
        if (it == _rawFactoryMap.cend())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_INVALID_RAW_HANDLER, "Invalid raw handler id.");
        }

        shared_ptr<IRawHandler> handler = it->second();
        handler->SetHandlerHost(_handlerHostProxy);
        _rawHandlerMap[id] = handler;

        return Json::Value();
    }

    Json::Value RawHandlerStub::DestroyHandler(
        const Json::Value& config)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        string id = config[JsonHostDestroyHandlerId].asString();

        RawHandlerMap::const_iterator it = _rawHandlerMap.find(id);
        if (it == _rawHandlerMap.cend())
        {
            return Json::Value();
        }
        it->second->Stop();
        _rawHandlerMap.erase(id);

        return Json::Value();
    }

    void RawHandlerStub::GetHandlersInfo(
        Json::Value& jsonList) const
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        for (const auto& it : _rawFactoryMap)
        {
            Json::Value handleInfo(Json::ValueType::objectValue);
            handleInfo[JsonHostGetHandlersInfoResultId] = Json::Value(it.first);
            handleInfo[JsonHostGetHandlersInfoResultType] = Json::Value(JsonHandlerTypeRaw);
            jsonList.append(handleInfo);
        }
    }

    Json::Value RawHandlerStub::RouteToHandler(
        const string& targetId,
        const string& targetMethod,
        const Json::Value& parameters)
    {
        RawHandlerMap::const_iterator it = _rawHandlerMap.find(targetId);
        if (_rawHandlerMap.cend() == it)
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_INVALID_RAW_HANDLER, "Unknown raw configuration handler.");
        }

        std::shared_ptr<DMCommon::IRawHandler> target = it->second;

        if (targetMethod == JsonHandlerStart)
        {
            return HandleStart(target, parameters);
        }
        else if (targetMethod == JsonHandlerStop)
        {
            return HandleStop(target, parameters);
        }
        else if (targetMethod == JsonHandlerOnConnectionStatusChanged)
        {
            return HandleOnConnectionStatusChanged(target, parameters);
        }
        else if (targetMethod == JsonGetHandlerType)
        {
            return HandleGetHandlerType(target, parameters);
        }
        else if (targetMethod == JsonRawIsConfigured)
        {
            return HandleIsConfigured(target, parameters);
        }
        else if (targetMethod == JsonRawGetDeploymentStatus)
        {
            return HandleGetDeploymentStatus(target, parameters);
        }
        else if (targetMethod == JsonRawSetDeploymentStatus)
        {
            return HandleSetDeploymentStatus(target, parameters);
        }
        else if (targetMethod == JsonRawInvoke)
        {
            return HandleInvoke(target, parameters);
        }
        else if (targetMethod == JsonRawGetDeploymentStatusJson)
        {
            return HandleGetDeploymentStatusJson(target, parameters);
        }
        else if (targetMethod == JsonRawGetReportedSchema)
        {
            return HandleGetReportedSchema(target, parameters);
        }
        throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_INVALID_RAW_METHOD, "Unknown raw configuration method.");
    }

    Json::Value RawHandlerStub::HandleStart(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        bool active = false;
        target->Start(parameters, active);

        Json::Value jsonResult(Json::ValueType::objectValue);
        jsonResult[JsonHandlerStartResult] = Json::Value(active);
        return jsonResult;
    }

    Json::Value RawHandlerStub::HandleStop(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        target->Stop();

        return Json::Value();
    }

    Json::Value RawHandlerStub::HandleOnConnectionStatusChanged(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        if (parameters.isNull() || !parameters.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_ARGUMENT, "Invalid input for HandleOnConnectionStatusChanged");
        }

        Json::Value valueJson = parameters[JsonHandlerConnectionStatusValue];
        if (valueJson.isNull() || !valueJson.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_ARGUMENT, "Invalid input schema for HandleOnConnectionStatusChanged");
        }

        ConnectionStatus connectionStatus = ConnectionStatusFromString(valueJson.asString());

        target->OnConnectionStatusChanged(connectionStatus);

        return Json::Value();
    }

    Json::Value RawHandlerStub::HandleGetHandlerType(
        std::shared_ptr<DMCommon::IRawHandler> target,
        const Json::Value& parameters)
    {
        string handlerType = target->GetHandlerType();

        Json::Value jsonResult(Json::ValueType::objectValue);
        jsonResult[JsonGetHandlerTypeResult] = Json::Value(handlerType);

        return jsonResult;
    }

    Json::Value RawHandlerStub::HandleIsConfigured(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        bool isConfigured = target->IsConfigured();

        Json::Value jsonResult(Json::ValueType::objectValue);
        jsonResult[JsonRawIsConfiguredResult] = Json::Value(isConfigured);

        return jsonResult;
    }

    Json::Value RawHandlerStub::HandleGetDeploymentStatus(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        string deploymentStatusString = DeploymentStatusToString(target->GetDeploymentStatus());

        Json::Value payload(Json::ValueType::objectValue);
        payload[JsonRawGetDeploymentStatusResult] = Json::Value(deploymentStatusString);

        return payload;
    }

    Json::Value RawHandlerStub::HandleSetDeploymentStatus(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        if (parameters.isNull() || !parameters.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_ARGUMENT, "Invalid input for HandleSetDeploymentStatus");
        }

        Json::Value valueJson = parameters[JsonRawSetDeploymentStatusValue];
        if (valueJson.isNull() || !valueJson.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_ARGUMENT, "Invalid input schema for HandleSetDeploymentStatus");
        }

        DeploymentStatus deploymentStatus = DeploymentStatusFromString(valueJson.asString());

        target->SetDeploymentStatus(deploymentStatus);

        return Json::Value();
    }

    Json::Value RawHandlerStub::HandleInvoke(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        InvokeResult invokeResult = target->Invoke(parameters);

        Json::Value returnPayload(Json::ValueType::objectValue);
        returnPayload[JsonInvokeResultPresent] = Json::Value(invokeResult.present);
        returnPayload[JsonInvokeResultCode] = Json::Value(invokeResult.code);

        // We need to parse the payload to avoid double escaping.
        Json::Value payloadJson;
        if (invokeResult.present)
        {
            Json::CharReaderBuilder builder;
            istringstream payloadStream(invokeResult.payload);
            string errs;
            if (!Json::parseFromStream(builder, payloadStream, &payloadJson, &errs))
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Failed to parse direct method returned payload. Not a valid json value.");
            }
        }

        returnPayload[JsonInvokeResultPayload] = payloadJson;

        return returnPayload;
    }

    Json::Value RawHandlerStub::HandleGetDeploymentStatusJson(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        Json::Value deploymentStatusJson = target->GetDeploymentStatusJson();

        Json::Value payload(Json::ValueType::objectValue);
        payload[JsonRawGetDeploymentStatusJsonResult] = deploymentStatusJson;

        return payload;
    }

    Json::Value RawHandlerStub::HandleGetReportedSchema(
        shared_ptr<IRawHandler> target,
        const Json::Value& parameters)
    {
        ReportedSchema reportedSchema = target->GetReportedSchema();

        Json::Value payload(Json::ValueType::objectValue);
        payload[JsonRawGetReportedSchemaResult] = reportedSchema.ToJsonValue();

        return payload;
    }

}}}}}
