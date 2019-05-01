// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include "DMJsonConstants.h"
#include "DMInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

class ReportedSummary
{
    typedef std::map<std::string, DeploymentStatus> StatusMap;

public:

    ReportedSummary();

    void SetSectionStatus(
        const std::string& sectionId,
        DeploymentStatus status
    ) noexcept;

    DeploymentStatus GetSectionStatus(
        const std::string& sectionName) const;

    void AddDeployment(
            const std::string& deploymentId);

    void RemoveDeployment(
            const std::string& deploymentId);

    DeploymentStatus GetDeploymentStatus() const;
    std::string GetId() const;

    Json::Value ToJsonObject() const;

    std::string ToJsonString() const;

private:

    std::string _id;
    DeploymentStatus _deploymentStatus;

    StatusMap _sectionStatusMap;
    std::set<std::string> _deployments;
};

}}}}
