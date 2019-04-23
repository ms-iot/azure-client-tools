// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "HandlerBase.h"
#include "Plugins/PluginConstants.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    HandlerBase::HandlerBase(
        const std::string& id,
        const ReportedSchema& reportedSchema) :
        _id(id),
        _isConfigured(false),
        _reportedSchema(reportedSchema),
        _iPluginHost(nullptr),
        _handlerConfig(Json::Value())
    {
        _metaData = make_shared<MetaData>();
        _metaData->SetDeploymentId(JsonDeploymentIdUnspecified);

        SetDeploymentStatus(DeploymentStatus::eNotStarted);
    }

    std::string HandlerBase::GetHandlerType() const
    {
        return JsonHandlerTypeRaw;
    }

    string HandlerBase::GetId() const
    {
        return _id;
    }

    bool HandlerBase::IsConfigured() const
    {
        return _isConfigured;
    }

    void HandlerBase::FinalizeAndReport(
        Json::Value& reportedObject,
        std::shared_ptr<ReportedErrorList> errorList)
    {
        DeploymentStatus deploymentStatus = errorList->Count() == 0 ? DeploymentStatus::eSucceeded : DeploymentStatus::eFailed;

        _metaData->SetDeploymentStatus(deploymentStatus);
        reportedObject[JsonMeta] = _metaData->ToJsonObject();
        reportedObject[JsonErrorList] = errorList->ToJsonObject();

        _iPluginHost->Report(GetId().c_str(), deploymentStatus, reportedObject);
    }

    void HandlerBase::SendEvent(string eventName, Json::Value& eventObject)
    {
        _iPluginHost->SendEvent(GetId().c_str(), eventName, eventObject);
    }

    void HandlerBase::ReportRefreshing()
    {
        _iPluginHost->Report(GetId().c_str(), DeploymentStatus::ePending, Json::Value(JsonRefreshing));
    }

    void HandlerBase::SetConfig(
        const Json::Value& handlerConfig)
    {
        _handlerConfig = handlerConfig;
    }

    Json::Value HandlerBase::GetConfig() const
    {
        return _handlerConfig;
    }

    void HandlerBase::Stop()
    {
        // default implementation.
    }

    DeploymentStatus HandlerBase::GetDeploymentStatus() const
    {
        return _metaData->GetDeploymentStatus();
    }

    void HandlerBase::SetDeploymentStatus(
        DeploymentStatus deploymentStatus)
    {
        _metaData->SetDeploymentStatus(deploymentStatus);

        Json::Value root(Json::objectValue);
        root[JsonMeta] = _metaData->ToJsonObject(JsonDeploymentStatus);
        _deploymentStatusJson = root;
    }

    Json::Value HandlerBase::GetDeploymentStatusJson() const
    {
        return _deploymentStatusJson;
    }

    ReportedSchema HandlerBase::GetReportedSchema() const
    {
        return _reportedSchema;
    }

    void HandlerBase::SetHandlerHost(
        std::shared_ptr<IRawHandlerHost> iPluginHost)
    {
        _iPluginHost = iPluginHost;
    }

}}}}

