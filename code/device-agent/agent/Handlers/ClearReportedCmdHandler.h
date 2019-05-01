// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <functional>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class ClearReportedCmdHandler : public DMCommon::HandlerBase
    {
    public:
        ClearReportedCmdHandler(
            const std::function<void(std::vector<std::string>& handlerIds)>& getHandlerIds,
            const std::function<void(const Json::Value& reportedProperties)>& reportAll);

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& jsonParameters) noexcept;

    private:
        const std::function<void(std::vector<std::string>& handlerIds)> GetHandlerIds;
        const std::function<void(const Json::Value& reportedProperties)> ReportAll;
    };

}}}}
