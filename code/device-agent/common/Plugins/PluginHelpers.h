// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "..\DMInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    std::string DeploymentStatusToString(DeploymentStatus deploymentStatus);
    DeploymentStatus DeploymentStatusFromString(const std::string& deploymentStatusString);

    std::string ConnectionStatusToString(ConnectionStatus connectionStatus);
    ConnectionStatus ConnectionStatusFromString(const std::string& connectionStatusString);

}}}}
