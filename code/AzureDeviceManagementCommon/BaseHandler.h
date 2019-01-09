// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include "DMInterfaces.h"
#include "CSPs\MdmProxy.h"
#include "MetaData.h"
#include "ReportedErrorList.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class BaseHandler : public IRawHandler
    {
    public:

        BaseHandler(
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
        void SetDeploymentStatus(DeploymentStatus deploymentStatus);

        // Deployment Status Json
        Json::Value GetDeploymentStatusJson() const;

        // Reported Schema
        ReportedSchema GetReportedSchema() const;

    protected:

        // Helpers
        bool RunOperation(
            const std::string& operationId,
            std::shared_ptr<ReportedErrorList> errorList,
            const std::function<void()>& Action);

        void SetMdmServer(
            std::shared_ptr<IMdmServer>  iMdmServer);

        void SetHandlerHost(
            std::shared_ptr<IRawHandlerHost> iPluginHost);

        bool IsRefreshing(
            const Json::Value& desiredConfig) const;

        void FinalizeAndReport(
            Json::Value& reportedObject,
            std::shared_ptr<ReportedErrorList> errorList);

        void SignalRefreshing();

        void SetConfig(
            const Json::Value& handlerConfig);

        Json::Value GetConfig() const;

        // ToDo: Better name/better params.
        static OperationModel TryGetOptionalSinglePropertyOpParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        static OperationModelT<int> TryGetOptionalSinglePropertyOpIntParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        static OperationModelT<bool> TryGetOptionalSinglePropertyOpBoolParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        static OperationModelT<std::string> TryGetOptionalSinglePropertyOpStringParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        std::string GetSinglePropertyOpStringParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        // Data members
        std::string _id;
        bool _isConfigured; // Has this handler received any configuration from the cloud?
        MdmProxy _mdmProxy;
        std::shared_ptr<IRawHandlerHost> _iPluginHost;

        // Configuration
        Json::Value _handlerConfig;

        // Deployment Status
        std::shared_ptr<MetaData> _metaData;
        Json::Value _deploymentStatusJson;

        // Reported Schema
        ReportedSchema _reportedSchema;
    };

}}}}
