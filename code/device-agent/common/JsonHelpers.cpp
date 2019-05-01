// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "JsonHelpers.h"
#include "DMJsonConstants.h"

using namespace std;
using namespace Microsoft::Azure::DeviceManagement::Utils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    int JsonHelpers::GetNamedInteger(const Json::Value& root, const string& name)
    {
        if (root.isNull())
        {
            throw DMException(JsonErrorSubsystemDeviceManagement, JsonErrorJsonParseError, "unexpected null json value.");
        }

        if (!root.isObject())
        {
            throw DMException(JsonErrorSubsystemDeviceManagement, JsonErrorJsonParseError, "unexpected json value type (object expected).");
        }

        Json::Value value = root[name];
        if (value.isNull())
        {
            throw DMException(JsonErrorSubsystemDeviceManagement, JsonErrorJsonParseError, "missing json value: " + name);
        }

        return value.asInt();
    }

    string JsonHelpers::GetNamedString(const Json::Value& root, const string& name)
    {
        if (root.isNull())
        {
            throw DMException(JsonErrorSubsystemDeviceManagement, JsonErrorJsonParseError, "unexpected null json value.");
        }

        if (!root.isObject())
        {
            throw DMException(JsonErrorSubsystemDeviceManagement, JsonErrorJsonParseError, "unexpected json value type (object expected).");
        }

        Json::Value value = root[name];
        if (value.isNull())
        {
            throw DMException(JsonErrorSubsystemDeviceManagement, JsonErrorJsonParseError, "missing json value: " + name);
        }

        return value.asString();
    }

    string JsonHelpers::GetNamedString(const Json::Value& root, const string& name, const string& defaultValue)
    {
        if (root.isNull())
        {
            throw DMException(JsonErrorSubsystemDeviceManagement, JsonErrorJsonParseError, "unexpected null json value.");
        }

        if (!root.isObject())
        {
            throw DMException(JsonErrorSubsystemDeviceManagement, JsonErrorJsonParseError, "unexpected json value type (object expected).");
        }

        Json::Value value = root[name];
        if (value.isNull())
        {
            return defaultValue;
        }

        return value.asString();
    }


    bool JsonHelpers::HasKey(const Json::Value& object, const string& name)
    {
        if (object.isNull() || !object.isObject())
        {
            return false;
        }
        Json::Value value = object[name];
        return !value.isNull();
    }

    void JsonHelpers::Merge(const Json::Value& source, Json::Value& target)
    {
        if (source.isNull())
        {
            return;
        }

        if (!source.isObject() || !target.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Both source and destination should be json objects for merging.");
        }

        vector<string> sourceKeys = source.getMemberNames();

        for (const string& sourceKey : sourceKeys)
        {
            const Json::Value& s = source[sourceKey];

            Json::Value& t = target[sourceKey];

            if (s.isObject())
            {
                if (t.isObject())
                {
                    Merge(s, t);
                }
                else
                {
                    t.copy(s);
                }
            }
            else
            {
                t.copy(s);
            }
        }
    }

    string JsonHelpers::DeploymentStatusToString(DeploymentStatus deploymentState)
    {
        string s;

        switch (deploymentState)
        {
        case eSucceeded:
            s = JsonDeploymentStatusSucceeded;
            break;
        case eNotStarted:
            s = JsonDeploymentStatusNotStarted;
            break;
        case ePending:
            s = JsonDeploymentStatusPending;
            break;
        case eFailed:
            s = JsonDeploymentStatusFailed;
            break;
        }

        return s;
    }

}}}}
