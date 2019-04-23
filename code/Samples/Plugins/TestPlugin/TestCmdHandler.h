// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "device-agent/common/MdmHandlerBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace TestPlugin {

    class TestCmdHandler : public DMCommon::MdmHandlerBase
    {
    public:
        TestCmdHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredConfig) noexcept;
    };

}}}}
