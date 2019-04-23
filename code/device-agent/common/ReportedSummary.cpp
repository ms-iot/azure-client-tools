// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DMJsonConstants.h"
#include "ReportedSummary.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

ReportedSummary::ReportedSummary() :
    _deploymentStatus(eNotStarted)
{
    _id = JsonReportedSummary;
}

DeploymentStatus ReportedSummary::GetSectionStatus(const string& sectionName) const
{
    StatusMap::const_iterator it = _sectionStatusMap.find(sectionName);

    if (it == _sectionStatusMap.cend())
    {
        // ToDo: should be more forgiving for unknown sections?
        throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_SECTION, "Undefined section name");
    }
    return it->second;
}

DeploymentStatus ReportedSummary::GetDeploymentStatus() const
{
    return _deploymentStatus;
}

string ReportedSummary::GetId() const
{
    return _id;
}

void ReportedSummary::SetSectionStatus(
    const std::string& sectionId,
    DeploymentStatus status) noexcept
{
    _sectionStatusMap[sectionId] = status;
}

void ReportedSummary::AddDeployment(
                            const string& deploymentId)
{
    _deployments.emplace(deploymentId);
}

void ReportedSummary::RemoveDeployment(
                            const string& deploymentId)
{
    set<string>::const_iterator found = _deployments.find(deploymentId);
    if (found != _deployments.cend())
    {
        _deployments.erase(found);
    }
}

Json::Value ReportedSummary::ToJsonObject() const
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    unsigned int failedCount = 0;
    string failedSections;

    unsigned int pendingCount = 0;
    string pendingSections;

    for (const auto& pair : _sectionStatusMap)
    {
        switch (pair.second)
        {
        case eFailed:
            ++failedCount;
            if (failedSections.size() != 0)
            {
                failedSections += ";";
            }
            failedSections += pair.first;
            break;
        case ePending:
            ++pendingCount;
            if (pendingSections.size() != 0)
            {
                pendingSections += ";";
            }
            pendingSections += pair.first;
            break;
        }
    }

    string deployments;
    for (const auto& s : _deployments)
    {
        if (deployments.size() != 0)
        {
            deployments += ";";
        }
        deployments += s;
    }

    Json::Value summaryObject(Json::objectValue);
    summaryObject[JsonFailedCount] = Json::Value(failedCount);
    summaryObject[JsonFailedGroups] = Json::Value(failedSections.c_str());
    summaryObject[JsonPendingCount] = Json::Value(pendingCount);
    summaryObject[JsonPendingGroups] = Json::Value(pendingSections);
    summaryObject[JsonDeployments] = Json::Value(deployments.c_str());
    summaryObject[JsonTime] = Json::Value(Utils::WideToMultibyte(DateTime::GetCurrentDateTimeString().c_str()));
    return summaryObject;
}

std::string ReportedSummary::ToJsonString() const
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    Json::Value summaryObject = ToJsonObject();

    return summaryObject.toStyledString();
}

}}}}
