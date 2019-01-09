// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginHelpers.h"
#include "PluginJsonConstants.h"

using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    string DeploymentStatusToString(DeploymentStatus deploymentStatus)
    {
        string valueString = JsonRawSetDeploymentStatusNotStarted;

        switch (deploymentStatus)
        {
        case eSucceeded:
            valueString = JsonRawSetDeploymentStatusSucceeded;
            break;
        case ePending:
            valueString = JsonRawSetDeploymentStatusPending;
            break;
        case eFailed:
            valueString = JsonRawSetDeploymentStatusFailed;
            break;
        default:
            throw DMException(-1, "Invalid deployment status value.");
        }

        return valueString;
    }

    DeploymentStatus DeploymentStatusFromString(const string& deploymentStatusString)
    {
        DeploymentStatus deploymentStatus = DeploymentStatus::eNotStarted;

        if (deploymentStatusString == JsonRawSetDeploymentStatusSucceeded)
        {
            deploymentStatus = DeploymentStatus::eSucceeded;
        }
        else if (deploymentStatusString == JsonRawSetDeploymentStatusPending)
        {
            deploymentStatus = DeploymentStatus::ePending;
        }
        else if (deploymentStatusString == JsonRawSetDeploymentStatusFailed)
        {
            deploymentStatus = DeploymentStatus::eFailed;
        }
        else
        {
            throw DMException(-1, "Invalid deployment status string value.");
        }
        return deploymentStatus;
    }


    string ConnectionStatusToString(ConnectionStatus connectionStatus)
    {
        string valueString = JsonConnectionStatusOffline;

        switch (connectionStatus)
        {
        case ConnectionStatus::eOffline:
            valueString = JsonConnectionStatusOffline;
            break;
        case ConnectionStatus::eOnline:
            valueString = JsonConnectionStatusOnline;
            break;
        default:
            throw DMException(-1, "ConnectionStatusToString. Bad connection status input.");
        }

        return valueString;
    }

    ConnectionStatus ConnectionStatusFromString(const string& connectionStatusString)
    {
        ConnectionStatus connectionStatus = ConnectionStatus::eOffline;

        if (connectionStatusString == JsonConnectionStatusOffline)
        {
            connectionStatus = ConnectionStatus::eOffline;
        }
        else if (connectionStatusString == JsonConnectionStatusOnline)
        {
            connectionStatus = ConnectionStatus::eOnline;
        }
        else
        {
            throw DMException(-1, "Invalid connection status string value.");
        }

        return connectionStatus;
    }

}}}}
