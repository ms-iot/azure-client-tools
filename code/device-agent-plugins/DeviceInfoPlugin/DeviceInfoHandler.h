// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "device-agent/common/MdmHandlerBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace DeviceInfoPlugin {

    class DeviceInfoHandler : public DMCommon::MdmHandlerBase
    {
    public:
        DeviceInfoHandler();

        void Start(
            const Json::Value& handlerConfig,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& groupDesiredConfigJson) noexcept;

    private:

        template<typename T>
        void ReadNumberProperty(
            const std::string& cspNodePath,
            const std::string& operationId,
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList)
        {
            Operation::RunOperation(operationId, errorList,
                [&]()
            {
                // Read back
                T value = static_cast<T>(_mdmProxy.RunGetUInt(cspNodePath));
                reportedObject[operationId] = Json::Value(value);
            });
        }

        void ReadStringProperty(
            const std::string& cspNodePath,
            const std::string& operationId,
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void BuildReported(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void EmptyReported(
            Json::Value& reportedObject);

    };

}}}}
