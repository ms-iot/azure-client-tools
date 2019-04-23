// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "JsonHelpers.h"
#include "MetaData.h"
#include "DMJsonConstants.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    MetaData::MetaData() :
        _deploymentStatus(DeploymentStatus::eNotStarted)
    {
        _dependencies = JsonDependenciesDefault;
        _deploymentId = JsonDeploymentIdDefault;
        _reportingMode = JsonReportingModeDefault;
    }

    void MetaData::SetDeploymentId(const std::string& deploymentId)
    {
        _deploymentId = deploymentId;
        _timeStamp = Utils::WideToMultibyte(DateTime::GetCurrentDateTimeString().c_str());
    }

    void MetaData::SetDeploymentStatus(DeploymentStatus deploymentStatus)
    {
        _deploymentStatus = deploymentStatus;
        _timeStamp = Utils::WideToMultibyte(DateTime::GetCurrentDateTimeString().c_str());
    }

    DeploymentStatus MetaData::GetDeploymentStatus() const
    {
        return _deploymentStatus;
    }

    void MetaData::FromJsonObject(
        const Json::Value& metaObject)
    {
        _dependencies = JsonHelpers::GetNamedString(metaObject, JsonDependencies, _dependencies /*default*/);
        _deploymentId = JsonHelpers::GetNamedString(metaObject, JsonDeploymentId, _deploymentId /*default*/);
        _reportingMode = JsonHelpers::GetNamedString(metaObject, JsonReportingMode, _reportingMode /*default*/);
    }

    void MetaData::FromJsonParentObject(
        const Json::Value& metaParentObject)
    {
        const Json::Value& metaObject = metaParentObject[JsonMeta];
        if (metaObject.isNull() || !metaObject.isObject())
        {
            return;
        }

        FromJsonObject(metaObject);
    }

    Json::Value MetaData::ToJsonObject() const
    {
        Json::Value metaObject(Json::objectValue);
        metaObject[JsonDeploymentId] = Json::Value(_deploymentId);
        metaObject[JsonDeploymentStatus] = Json::Value(JsonHelpers::DeploymentStatusToString(_deploymentStatus));
        metaObject[JsonTime] = Json::Value(_timeStamp);
        return metaObject;
    }

    std::string MetaData::ToJsonString() const
    {
        Json::Value metaObject = ToJsonObject();

        return metaObject.toStyledString();
    }

    Json::Value MetaData::ToJsonObject(
        const std::string& propertyName) const
    {
        Json::Value metaObject(Json::objectValue);
        if (propertyName == JsonDeploymentId)
        {
            metaObject[JsonDeploymentId] = Json::Value(_deploymentId);
        }
        else if (propertyName == JsonDeploymentStatus)
        {
            metaObject[JsonDeploymentStatus] = Json::Value(JsonHelpers::DeploymentStatusToString(_deploymentStatus));
        }
        else if (propertyName == JsonTime)
        {
            metaObject[JsonTime] = Json::Value(_timeStamp);
        }

        return metaObject;
    }

    Json::Value MetaData::DeploymentStatusToJsonObject(
        DeploymentStatus deploymentStatus)
    {
        Json::Value metaObject(Json::objectValue);
        metaObject[JsonDeploymentStatus] = Json::Value(JsonHelpers::DeploymentStatusToString(deploymentStatus));
        return metaObject;
    }

    std::string MetaData::ToJsonString(
        const std::string& propertyName) const
    {
        Json::Value metaObject = ToJsonObject(propertyName);

        return metaObject.toStyledString();
    }

    std::string MetaData::GetDependencies() const
    {
        return _dependencies;
    }

    std::string MetaData::GetDeploymentId() const
    {
        return _deploymentId;
    }

    void MetaData::SetReportingMode(const std::string& reportingMode)
    {
        _reportingMode = reportingMode;
    }

    std::string MetaData::GetReportingMode() const
    {
        return _reportingMode;
    }

}}}}
