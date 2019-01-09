// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace TimePlugin {

    class TimeStateHandler : public DMCommon::BaseHandler
    {
    public:
        TimeStateHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& groupDesiredConfigJson) noexcept;

    private:

        static void SetNtpServer(
            const std::string& ntpServer);

        static std::string GetNtpServer();

        void GetSubGroupNtpServer(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void SetSubGroupNtpServer(
            const Json::Value& groupDesiredConfigJson,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void GetSubGroupTimeZone(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void SetSubGroupTimeZone(
            const Json::Value& desiredConfig,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void BuildReported(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void EmptyReported(
            Json::Value& reportedObject);

        Json::Value _groupDesiredConfigJson;
    };

}}}}
