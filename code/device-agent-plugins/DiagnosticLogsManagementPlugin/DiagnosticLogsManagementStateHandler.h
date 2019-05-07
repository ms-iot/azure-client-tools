// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "EventTracingConfig.h"

#include "device-agent/common/MdmHandlerBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace DiagnosticLogsManagementPlugin {

    class DiagnosticLogsManagementStateHandler : public DMCommon::MdmHandlerBase
    {
    public:
        DiagnosticLogsManagementStateHandler();

        // IRawHandler
        void Start(
            const Json::Value& handlerConfig,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& groupDesiredConfigJson) noexcept;

    private:
        std::string _dataFolder;

        void CreateEtlFile(
            CollectorConfiguration collector);

        std::vector<std::shared_ptr<CollectorConfiguration>> GetSubGroup();

        void ApplyCollectorConfiguration(
            CollectorConfiguration collector,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void SetSubGroup(
            const Json::Value& desiredConfig,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList,
            std::set<std::string>& configuredCollectors);

        void BuildReported(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList,
            const std::set<std::string>& configuredCollectors);
    };

}}}}
