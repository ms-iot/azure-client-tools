// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "RawHandlerProxy.h"
#include "../../common/plugins/PluginJsonConstants.h"
#include "../../common/plugins/PluginHelpers.h"

using namespace std;
using namespace DMUtils;
using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    RawHandlerProxy::RawHandlerProxy(
        const string& id,
        std::shared_ptr<DMCommon::IPluginBinaryProxy> binaryProxy) :
        _id(id),
        _binaryProxy(binaryProxy)
    {}


    std::string RawHandlerProxy::GetHandlerType() const
    {
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonGetHandlerType, Json::Value());

        Json::Value jsonResult = payload[JsonGetHandlerTypeResult];
        if (jsonResult.isNull() || !jsonResult.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "RawHandlerProxy::GetHandlerType. Bad result format.");
        }

        return jsonResult.asString();
    }

    string RawHandlerProxy::GetId() const
    {
        return _id;
    }

    // ToDo: Move to a common base.
    void RawHandlerProxy::Start(
        const Json::Value& config,
        bool& active)
    {
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonHandlerStart, config);

        Json::Value jsonResult = payload[JsonHandlerStartResult];
        if (jsonResult.isNull() || !jsonResult.isBool())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "RawHandlerProxy::Start. Bad result format.");
        }

        active = jsonResult.asBool();
    }

    // ToDo: Move to a common base.
    void RawHandlerProxy::Stop()
    {
        _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonHandlerStop, Json::Value());
    }

    void RawHandlerProxy::OnConnectionStatusChanged(
        ConnectionStatus status)
    {
        Json::Value parametersJson(Json::ValueType::objectValue);
        parametersJson[JsonHandlerConnectionStatusValue] = ConnectionStatusToString(status);

        _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonHandlerOnConnectionStatusChanged, parametersJson);
    }

    bool RawHandlerProxy::IsConfigured() const
    {
        bool isConfigured = 0;

        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawIsConfigured, Json::Value());

        Json::Value jsonResult = payload[JsonRawIsConfiguredResult];
        if (jsonResult.isNull() || !jsonResult.isInt())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "RawHandlerProxy::IsConfigured. Bad result format.");
        }
        isConfigured = jsonResult.asBool();

        return isConfigured;
    }

    void RawHandlerProxy::SetDeploymentStatus(DeploymentStatus deploymentStatus)
    {
        Json::Value parametersJson(Json::ValueType::objectValue);
        parametersJson[JsonRawSetDeploymentStatusValue] = DeploymentStatusToString(deploymentStatus);

        _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawSetDeploymentStatus, parametersJson);
    }

    DeploymentStatus RawHandlerProxy::GetDeploymentStatus() const
    {
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawGetDeploymentStatus, Json::Value());

        Json::Value jsonResult = payload[JsonRawGetDeploymentStatusResult];
        if (jsonResult.isNull() || !jsonResult.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "RawHandlerProxy::GetDeploymentStatus. Bad result format.");
        }

        return DeploymentStatusFromString(jsonResult.asString());
    }

    InvokeResult RawHandlerProxy::Invoke(const Json::Value& parametersJson)
    {
        InvokeResult invokeResult;
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawInvoke, parametersJson);
        invokeResult.present = payload[JsonInvokeResultPresent].asBool();
        invokeResult.code = payload[JsonInvokeResultCode].asInt();
        invokeResult.payload = payload[JsonInvokeResultPayload].toStyledString();

        return invokeResult;
    }

    Json::Value RawHandlerProxy::GetDeploymentStatusJson() const
    {
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawGetDeploymentStatusJson, Json::Value());

        Json::Value jsonResult = payload[JsonRawGetDeploymentStatusJsonResult];
        if (jsonResult.isNull() || !jsonResult.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "RawHandlerProxy::GetDeploymentStatusJson. Bad result format.");
        }
        return jsonResult;
    }

    ReportedSchema RawHandlerProxy::GetReportedSchema() const
    {
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawGetReportedSchema, Json::Value());

        Json::Value jsonResult = payload[JsonRawGetReportedSchemaResult];
        if (jsonResult.isNull() || !jsonResult.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "RawHandlerProxy::GetReportedSchema. Bad result format.");
        }

        ReportedSchema reportedSchema;
        reportedSchema.FromJsonValue(jsonResult);
        return reportedSchema;
    }

    void RawHandlerProxy::SetHandlerHost(shared_ptr<IRawHandlerHost> iPluginHost)
    {
        (void)iPluginHost;
        // This is being set in the stub by the stub creator.
    }

}}}}
