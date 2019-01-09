// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "RawHandlerProxy.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginHelpers.h"

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
            throw DMException(-1, "RawHandlerProxy::GetHandlerType. Bad result format.");
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
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawStart, config);

        Json::Value jsonResult = payload[JsonRawStartResult];
        if (jsonResult.isNull() || !jsonResult.isBool())
        {
            throw DMException(-1, "RawHandlerProxy::Start. Bad result format.");
        }

        active = jsonResult.asBool();
    }

    // ToDo: Move to a common base.
    void RawHandlerProxy::Stop()
    {
        _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawStop, Json::Value());
    }

    void RawHandlerProxy::OnConnectionStatusChanged(
        ConnectionStatus status)
    {
        Json::Value parametersJson(Json::ValueType::objectValue);
        parametersJson[JsonConnectionStatusValue] = ConnectionStatusToString(status);

        _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawOnConnectionStatusChanged, parametersJson);
    }

    bool RawHandlerProxy::IsConfigured() const
    {
        bool isConfigured = 0;

        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawIsConfigured, Json::Value());

        Json::Value jsonResult = payload[JsonRawIsConfiguredResult];
        if (jsonResult.isNull() || !jsonResult.isInt())
        {
            throw DMException(-1, "RawHandlerProxy::IsConfigured. Bad result format.");
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
            throw DMException(-1, "RawHandlerProxy::GetDeploymentStatus. Bad result format.");
        }

        return DeploymentStatusFromString(jsonResult.asString());
    }

    InvokeResult RawHandlerProxy::Invoke(const Json::Value& parametersJson) noexcept
    {
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawInvoke, parametersJson);

        InvokeResult invokeResult;
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
            throw DMException(-1, "RawHandlerProxy::GetDeploymentStatusJson. Bad result format.");
        }
        return jsonResult;
    }

    ReportedSchema RawHandlerProxy::GetReportedSchema() const
    {
        Json::Value payload = _binaryProxy->Invoke(JsonTargetTypeRaw, GetId(), JsonRawGetReportedSchema, Json::Value());

        Json::Value jsonResult = payload[JsonRawGetReportedSchemaResult];
        if (jsonResult.isNull() || !jsonResult.isObject())
        {
            throw DMException(-1, "RawHandlerProxy::GetReportedSchema. Bad result format.");
        }

        ReportedSchema reportedSchema;
        reportedSchema.FromJsonValue(jsonResult);
        return reportedSchema;
    }

    void RawHandlerProxy::SetMdmServer(shared_ptr<IMdmServer> iMdmServer)
    {
        (void)iMdmServer;
        // This is being set in the stub by the stub creator.
    }

    void RawHandlerProxy::SetHandlerHost(shared_ptr<IRawHandlerHost> iPluginHost)
    {
        (void)iPluginHost;
        // This is being set in the stub by the stub creator.
    }

}}}}
