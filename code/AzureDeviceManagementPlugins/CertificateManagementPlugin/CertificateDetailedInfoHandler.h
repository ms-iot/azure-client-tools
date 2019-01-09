// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include <functional>
#include <list>
#include "CertificateFile.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace CertificateManagementPlugin {

    class CertificateDetailedInfoHandler : public DMCommon::BaseHandler
    {
    public:
        CertificateDetailedInfoHandler();

        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& jsonParameters) noexcept;
    private:

        void GetCertificateDetailHandler(
            const Json::Value& desiredConfig,
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);
    };

}}}}
