// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "..\..\..\AzureDeviceManagementClient\IotHub\IoTHubInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace RebootManagementPlugin {

    class RebootCmdHandler : public DMCommon::BaseHandler
    {
    public:
        RebootCmdHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& jsonParameters) noexcept;
    };

}}}}
