// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include "DMInterfaces.h"
#include "MetaData.h"
#include "ReportedErrorList.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class HandlerBase : public IRawHandler
    {
    public:

        HandlerBase(
            const std::string& id,
            const ReportedSchema& reportedSchema);

        // IHandler
        std::string GetHandlerType() const;

        // IRawHandler
        std::string GetId() const;
        bool IsConfigured() const;

        void Stop();

        // Deployment Status...
        DeploymentStatus GetDeploymentStatus() const;

        void SetDeploymentStatus(
            DeploymentStatus deploymentStatus);

        // Deployment Status Json
        Json::Value GetDeploymentStatusJson() const;

        // Reported Schema
        ReportedSchema GetReportedSchema() const;

    protected:

        // Helpers
        void SetHandlerHost(
            std::shared_ptr<IRawHandlerHost> iPluginHost);

        void FinalizeAndReport(
            Json::Value& reportedObject,
            std::shared_ptr<ReportedErrorList> errorList);

        void SendEvent(
            std::string eventName,
            Json::Value& eventObject);

        void ReportRefreshing();

        void SetConfig(
            const Json::Value& handlerConfig);

        Json::Value GetConfig() const;

        // Data members
        bool _isConfigured; // Has this handler received any configuration from the cloud?

        // Deployment Status
        std::shared_ptr<MetaData> _metaData;

    private:
        std::string _id;

        std::shared_ptr<IRawHandlerHost> _iPluginHost;

        Json::Value _deploymentStatusJson;

        // Configuration
        Json::Value _handlerConfig;

        // Reported Schema
        ReportedSchema _reportedSchema;
    };

}}}}
