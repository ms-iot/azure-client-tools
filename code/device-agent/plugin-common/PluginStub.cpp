// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "stdafx.h"
#include "PluginStub.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    PluginStub::PluginStub()
    {
#if DEBUG_DEVICE_AGENT_ROUTING
        gLogger.SetLogFilePath(L"", wstring(L"") + to_wstring(GetCurrentProcessId()) + L"_");
        gLogger.EnableConsole(true);
#endif
    }

    void PluginStub::Initialize()
    {
        RegisterHostFunction(JsonHostGetHandlersInfo, [&](const Json::Value& parameters) {
            return GetHandlersInfo();
        });

        RegisterHostFunction(JsonHostCreateRawHandler, [&](const Json::Value& parameters) {
            return _rawHandlerStub.CreateHandler(parameters);
        });

        RegisterHostFunction(JsonHostDestroyRawHandler, [&](const Json::Value& parameters) {
            return _rawHandlerStub.DestroyHandler(parameters);
        });

        _mdmServerProxy = make_shared<MdmServerProxy>(&_agentBinaryProxy);
        _rawHandlerHostProxy = make_shared<RawHandlerHostProxy>(&_agentBinaryProxy, _mdmServerProxy);

        _rawHandlerStub.SetAgentBinaryProxy(&_agentBinaryProxy);
        _rawHandlerStub.SetHandlerHostProxy(_rawHandlerHostProxy);
    }

    void PluginStub::RegisterRawHandler(
        const string& id,
        function<shared_ptr<IRawHandler>()> createFactory)
    {
        _rawHandlerStub.RegisterHandler(id, createFactory);
    }

    void PluginStub::UnregisterRawHandler(
        const string& id)
    {
        _rawHandlerStub.UnregisterHandler(id);
    }

    void PluginStub::SetReverseInvoke(
        PluginReverseInvokePtr ReverseInvoke_) noexcept
    {
        _agentBinaryProxy.SetReverseInvoke(ReverseInvoke_);
    }

    void PluginStub::SetReverseDeleteBuffer(
        PluginReverseDeleteBufferPtr ReverseDeleteBuffer_) noexcept
    {
        _agentBinaryProxy.SetReverseDeleteBuffer(ReverseDeleteBuffer_);
    }

    void PluginStub::RegisterHostFunction(
        const string& targetMethod,
        const EntryPoint& entryPoint)
    {
        _hostFunctionMap[targetMethod] = entryPoint;
    }

    //
    // Invoke()
    //  Serializes/Deserializes cross-binary objects and exceptions.
    //
    // Inputs
    //   jsonInputString : CrossBinaryRequest serialized into a json string.
    //
    // Returns:
    //   CrossBinaryResponse serialized into a json string.
    //
    string PluginStub::Invoke(
        const string& jsonInputString) noexcept
    {
        string resultString;

        try
        {
            // Deserialize the string into a request.
            CrossBinaryRequest request = CrossBinaryRequest::FromJsonString(jsonInputString);

            // Invoke
            Json::Value jsonOutput = Invoke(request.targetType, request.targetId, request.targetMethod, request.targetParameters);

            // Serialize the result into a response.
            resultString = CrossBinaryResponse::CreateFromSuccess(jsonOutput).ToJson().toStyledString();
        }
        catch (const DMException& ex)
        {
            resultString = CrossBinaryResponse::CreateFromException(ex).ToJson().toStyledString();
        }
        catch (const exception& ex)
        {
            resultString = CrossBinaryResponse::CreateFromException(ex).ToJson().toStyledString();
        }

        return resultString;
    }

    //
    // Invoke()
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
    Json::Value PluginStub::Invoke(
        const std::string& targetType,
        const std::string& targetId,
        const std::string& targetMethod,
        const Json::Value& targetParameters)
    {
        Json::Value result;

        if (targetType == JsonTargetTypeHost)
        {
            FunctionMap::const_iterator it = _hostFunctionMap.find(targetMethod);
            if (_hostFunctionMap.cend() == it)
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_INVALID_PLUGINHOST_OPERTION, "Unknown host operation.");
            }
            return it->second(targetParameters);
        }
        else if (targetType == JsonTargetTypeRaw)
        {
            return _rawHandlerStub.RouteToHandler(targetId, targetMethod, targetParameters);
        }

        throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_INVALID_OPERATION, "Unknown host operation.");
    }

    Json::Value PluginStub::GetHandlersInfo() const
    {
        Json::Value jsonList(Json::ValueType::arrayValue);
        _rawHandlerStub.GetHandlersInfo(jsonList);

        Json::Value jsonResult(Json::ValueType::objectValue);
        jsonResult[JsonHostGetHandlersInfoResult] = jsonList;

        return jsonResult;
    }

}}}}}
