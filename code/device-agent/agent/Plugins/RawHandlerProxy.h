// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>

#include "../../common/DMInterfaces.h"
#include "../../common/plugins/PluginInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class RawHandlerProxy : public DMCommon::IRawHandler
    {
    public:
        RawHandlerProxy(
            const std::string& id,
            std::shared_ptr<DMCommon::IPluginBinaryProxy> binaryProxy);

        std::string GetId() const;

        std::string GetHandlerType() const;

        void Start(
            const Json::Value& config, bool& active);

        void Stop();

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        bool IsConfigured() const;

        void SetDeploymentStatus(
            DMCommon::DeploymentStatus deploymentStatus);

        DMCommon::DeploymentStatus GetDeploymentStatus() const;

        DMCommon::InvokeResult Invoke(
            const Json::Value& parameters);

        Json::Value GetDeploymentStatusJson() const;

        DMCommon::ReportedSchema GetReportedSchema() const;

        void SetHandlerHost(
            std::shared_ptr<DMCommon::IRawHandlerHost> iPluginHost);

    private:
        std::string _id;
        std::shared_ptr<DMCommon::IPluginBinaryProxy> _binaryProxy;
    };

}}}}
