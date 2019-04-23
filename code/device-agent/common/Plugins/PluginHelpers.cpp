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
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_DEPLOYMENT_STATUS, "Invalid deployment status value.");
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
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_DEPLOYMENT_STATUS, "Invalid deployment status string value.");
        }
        return deploymentStatus;
    }


    string ConnectionStatusToString(ConnectionStatus connectionStatus)
    {
        string valueString = JsonHandlerConnectionStatusOffline;

        switch (connectionStatus)
        {
        case ConnectionStatus::eOffline:
            valueString = JsonHandlerConnectionStatusOffline;
            break;
        case ConnectionStatus::eOnline:
            valueString = JsonHandlerConnectionStatusOnline;
            break;
        default:
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_CONNECTION_STATUS, "ConnectionStatusToString. Bad connection status input.");
        }

        return valueString;
    }

    ConnectionStatus ConnectionStatusFromString(const string& connectionStatusString)
    {
        ConnectionStatus connectionStatus = ConnectionStatus::eOffline;

        if (connectionStatusString == JsonHandlerConnectionStatusOffline)
        {
            connectionStatus = ConnectionStatus::eOffline;
        }
        else if (connectionStatusString == JsonHandlerConnectionStatusOnline)
        {
            connectionStatus = ConnectionStatus::eOnline;
        }
        else
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_CONNECTION_STATUS, "Invalid connection status string value.");
        }

        return connectionStatus;
    }

}}}}
