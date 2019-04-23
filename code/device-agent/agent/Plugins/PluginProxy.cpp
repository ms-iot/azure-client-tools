// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "../../common/plugins/PluginJsonConstants.h"
#include "PluginProxy.h"
#include "PluginBinaryProxy.h"
#include "OutOfProcPluginBinaryProxy.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    PluginProxy::PluginProxy(const string& pluginFullPath, bool outOfProc, long keepAliveTime)
    {
        if (outOfProc)
        {
            _binaryProxy = std::make_shared<OutOfProcPluginBinaryProxy>(pluginFullPath, keepAliveTime);
        }
        else
        {
            _binaryProxy = std::make_shared<PluginBinaryProxy>(pluginFullPath);
        }
    }

    void PluginProxy::Load()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        _binaryProxy->Load();
    }

    void PluginProxy::Unload()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        _binaryProxy->Unload();
    }

    vector<DMCommon::HandlerInfo> PluginProxy::GetHandlersInfo() const
    {
        // Invoke() throws if errors are returned.
        Json::Value returnJson = _binaryProxy->Invoke(JsonTargetTypeHost, "", JsonHostGetHandlersInfo, Json::Value());

        Json::Value jsonResult = returnJson[JsonHostGetHandlersInfoResult];
        if (jsonResult.isNull() || !jsonResult.isArray())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_RESPONSE_JSON_SCHEMA, "PluginProxy::GetHandlersInfo received invalid json.");
        }

        vector<HandlerInfo> infoVector;
        for (Json::Value::const_iterator it = jsonResult.begin(); it != jsonResult.end(); ++it)
        {
            if (!it->isObject())
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_RESPONSE_JSON_SCHEMA, "PluginProxy::GetHandlersInfo received invalid json.");
            }

            const Json::Value& infoObject = *it;

            HandlerInfo handlerInfo;
            handlerInfo.id = infoObject[JsonHostGetHandlersInfoResultId].asString();
            handlerInfo.type = infoObject[JsonHostGetHandlersInfoResultType].asString();

            infoVector.emplace_back(handlerInfo);
        }

        return infoVector;
    }

    shared_ptr<IRawHandler> PluginProxy::CreateRawHandler(const string& id)
    {
        Json::Value parameters(Json::ValueType::objectValue);
        parameters[JsonHostCreateHandlerId] = Json::Value(id);

        // Invoke() throws if errors are returned.
        _binaryProxy->Invoke(JsonTargetTypeHost, "", JsonHostCreateRawHandler, parameters);

        return make_shared<RawHandlerProxy>(id, _binaryProxy);
    }

    void PluginProxy::DestroyRawHandler(const string& id)
    {
        Json::Value parameters(Json::ValueType::objectValue);
        parameters[JsonHostDestroyHandlerId] = Json::Value(id);

        // Invoke() throws if errors are returned.
        _binaryProxy->Invoke(JsonTargetTypeHost, "", JsonHostDestroyRawHandler, parameters);
    }

}}}}
