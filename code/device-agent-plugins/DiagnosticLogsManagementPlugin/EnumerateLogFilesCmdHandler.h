// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace DiagnosticLogsManagementPlugin {

    class EnumerateLogFiles : public DMCommon::HandlerBase
    {
    public:
        EnumerateLogFiles();

        // IRawHandler
        void Start(
            const Json::Value& handlerConfig,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredConfig) noexcept;

    private:
        std::string _dataFolder;
    };

}}}}
