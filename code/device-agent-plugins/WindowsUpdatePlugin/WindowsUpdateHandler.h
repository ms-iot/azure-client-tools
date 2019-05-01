// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "device-agent/common/MdmHandlerBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace WindowsUpdatePlugin {

    class WindowsUpdateHandler : public DMCommon::MdmHandlerBase
    {
    public:
        WindowsUpdateHandler();

        void Start(
            const Json::Value& handlerConfig,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredDeltaConfig) noexcept;

    private:

        void GetSubGroup(
            const std::string& cspNodeId,
            const std::string& operationId,
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void SetSubGroup(
            const Json::Value& groupDesiredConfigJson,
            const std::string& cspNodeId,
            const std::string& operationId,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void BuildReported(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void EmptyReported(
            Json::Value& reportedObject);
    };

}}}}
