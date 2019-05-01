// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "DMInterfaces.h"
#include "DMJsonConstants.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class MetaData
    {
    public:
        MetaData();

        // Setters
        void SetDeploymentId(const std::string& deploymentId);

        void SetDeploymentStatus(DeploymentStatus deploymentState);

        void SetReportingMode(const std::string& reportingMode);

        void SetDeviceInterfaceVersion(const std::string& deviceInterfaceVersion);

        // Getters

        DeploymentStatus GetDeploymentStatus() const;

        std::string GetDeploymentId() const;

        std::string GetReportingMode() const;

        std::string GetServiceInterfaceVersion() const;

        // Json Conversions

        void FromJsonObject(
            const Json::Value& metaObject);

        void FromJsonParentObject(
            const Json::Value& metaParentObject);

        void FromJsonObjectSubMeta(
            const Json::Value& metaObject);

        void FromJsonParentObjectSubMeta(
            const Json::Value& metaParentObject);

        Json::Value ToJsonObject() const;

        std::string ToJsonString() const;

        Json::Value ToJsonObject(
            const std::string& propertyName) const;

        std::string ToJsonString(
            const std::string& propertyName) const;

        // Helpers

        static Json::Value DeploymentStatusToJsonObject(
            DeploymentStatus deploymentStatus);

    private:
        std::string _deploymentId;          // desired/reported
        std::string _reportingMode;         // desired/reported
        std::string _serviceInterfaceVersion; // desired
        std::string _deviceInterfaceVersion;         // reported
        DeploymentStatus _deploymentStatus; // reported
        std::string _timeStamp;             // reported
    };

}}}}
