// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace WindowsTelemetryPlugin {

    class WindowsTelemetryStateHandler : public DMCommon::BaseHandler
    {
    public:
        WindowsTelemetryStateHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& groupDesiredConfigJson) noexcept;

    private:

        unsigned long StringToLevel(
            const std::string& levelString);

        std::string LevelToString(
            unsigned long level);

        void GetSubGroupLevel(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void SetSubGroupLevel(
            const Json::Value& groupDesiredConfigJson,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void BuildReported(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void EmptyReported(
            Json::Value& reportedObject);

    };

}}}}
