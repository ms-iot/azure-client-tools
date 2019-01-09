// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "..\IoThub\IoTHubInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class ClearReportedCmdHandler : public DMCommon::BaseHandler
    {
    public:
        ClearReportedCmdHandler(IDeviceTwin* deviceTwin);

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& jsonParameters) noexcept;

    private:
        IDeviceTwin* _deviceTwin;
    };

}}}}
