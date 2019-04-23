// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "device-agent/common/MdmHandlerBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace RebootPlugin {

    class RebootStateHandler : public DMCommon::MdmHandlerBase
    {
    public:
        RebootStateHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& groupDesiredConfigJson) noexcept;

    private:

        void SetSubGroup(
            const Json::Value& desiredConfig,
            const std::string& cspNodeId,
            const std::string& operationId,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void GetSubGroup(
            const std::string& cspNodeId,
            const std::string& operationId,
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void BuildReported(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void EmptyReported(
            Json::Value& reportedObject);

        std::string _lastRebootTime;
    };

}}}}
