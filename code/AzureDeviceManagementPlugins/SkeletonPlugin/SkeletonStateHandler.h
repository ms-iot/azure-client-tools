// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace SkeletonPlugin {

    class SkeletonStateHandler : public DMCommon::BaseHandler
    {
    public:
        SkeletonStateHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& groupDesiredConfigJson) noexcept;

    private:

        void BuildReported(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void EmptyReported(
            Json::Value& reportedObject);

    };

}}}}
