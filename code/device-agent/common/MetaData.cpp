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
        _deploymentId = JsonDeploymentIdDefault;
        _reportingMode = JsonReportingModeDefault;
    }

    void MetaData::SetDeploymentId(const string& deploymentId)
    {
        _deploymentId = deploymentId;
        _timeStamp = Utils::WideToMultibyte(DateTime::GetCurrentDateTimeString().c_str());
    }

    void MetaData::SetDeploymentStatus(DeploymentStatus deploymentStatus)
    {
        _deploymentStatus = deploymentStatus;
        _timeStamp = Utils::WideToMultibyte(DateTime::GetCurrentDateTimeString().c_str());
    }

    void MetaData::SetReportingMode(const string& reportingMode)
    {
        _reportingMode = reportingMode;
    }

    void MetaData::SetDeviceInterfaceVersion(const string& deviceInterfaceVersion)
    {
        VersionFormatCheck(deviceInterfaceVersion);
        _deviceInterfaceVersion = deviceInterfaceVersion;
    }

    void MetaData::FromJsonObject(
        const Json::Value& metaObject)
    {
        _deploymentId = JsonHelpers::GetNamedString(metaObject, JsonDeploymentId, _deploymentId /*default*/);
        _reportingMode = JsonHelpers::GetNamedString(metaObject, JsonReportingMode, _reportingMode /*default*/);
        _serviceInterfaceVersion = JsonHelpers::GetNamedString(metaObject, JsonServiceInterfaceVersion);
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

    void MetaData::FromJsonObjectSubMeta(
        const Json::Value& metaObject)
    {
        _reportingMode = JsonHelpers::GetNamedString(metaObject, JsonReportingMode, _reportingMode /*default*/);
    }

    void MetaData::FromJsonParentObjectSubMeta(
        const Json::Value& metaParentObject)
    {
        const Json::Value& metaObject = metaParentObject[JsonMeta];
        if (metaObject.isNull() || !metaObject.isObject())
        {
            return;
        }

        FromJsonObjectSubMeta(metaObject);
    }

    Json::Value MetaData::ToJsonObject() const
    {
        Json::Value metaObject(Json::objectValue);
        metaObject[JsonDeploymentId] = Json::Value(_deploymentId);
        metaObject[JsonDeploymentStatus] = Json::Value(JsonHelpers::DeploymentStatusToString(_deploymentStatus));
        metaObject[JsonTime] = Json::Value(_timeStamp);
        metaObject[JsonDeviceInterfaceVersion] = Json::Value(_deviceInterfaceVersion);

        return metaObject;
    }

    string MetaData::ToJsonString() const
    {
        Json::Value metaObject = ToJsonObject();

        return metaObject.toStyledString();
    }

    Json::Value MetaData::ToJsonObject(
        const string& propertyName) const
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
        else if (propertyName == JsonDeviceInterfaceVersion)
        {
            metaObject[JsonDeviceInterfaceVersion] = Json::Value(_deviceInterfaceVersion);
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

    string MetaData::ToJsonString(
        const string& propertyName) const
    {
        Json::Value metaObject = ToJsonObject(propertyName);

        return metaObject.toStyledString();
    }

    string MetaData::GetDeploymentId() const
    {
        return _deploymentId;
    }

    DeploymentStatus MetaData::GetDeploymentStatus() const
    {
        return _deploymentStatus;
    }

    string MetaData::GetReportingMode() const
    {
        return _reportingMode;
    }

    string MetaData::GetServiceInterfaceVersion() const
    {
        return _serviceInterfaceVersion;
    }

}}}}