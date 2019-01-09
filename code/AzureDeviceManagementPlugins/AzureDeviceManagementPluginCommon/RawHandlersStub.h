// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "..\..\AzureDeviceManagementCommon\DMInterfaces.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginInterfaces.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\CrossBinaryResponse.h"
#include "RawHandlerHostProxy.h"
#include "MdmServerProxy.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    class RawHandlerStub
    {
        typedef std::map<std::string, std::shared_ptr<DMCommon::IRawHandler>> RawHandlerMap;
        typedef std::map<std::string, std::function<std::shared_ptr<DMCommon::IRawHandler>()>> RawHandlerFactoryMap;

    public:
        RawHandlerStub();

        void SetAgentBinaryProxy(
            AgentBinaryProxy* agentBinaryProxy);

        void SetHandlerHostProxy(
            std::shared_ptr<RawHandlerHostProxy> handlerHostProxy);

        void SetMdmServerProxy(
            std::shared_ptr<MdmServerProxy> mdmServerProxy);

        void RegisterHandler(
            const std::string& id,
            std::function<std::shared_ptr<DMCommon::IRawHandler>()> createFactory);

        void UnregisterHandler(
            const std::string& id);

        Json::Value CreateHandler(
            const Json::Value& config);

        Json::Value DestroyHandler(
            const Json::Value& config);

        Json::Value RouteToHandler(
            const std::string& targetId,
            const std::string& targetMethod,
            const Json::Value& parameters);

        void GetHandlersInfo(
            Json::Value& jsonList) const;

    private:

        Json::Value HandleGetHandlerType(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleIsConfigured(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleStart(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleStop(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleOnConnectionStatusChanged(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleGetDeploymentStatus(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleSetDeploymentStatus(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleInvoke(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleGetDeploymentStatusJson(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        Json::Value HandleGetReportedSchema(
            std::shared_ptr<DMCommon::IRawHandler> target,
            const Json::Value& parameters);

        AgentBinaryProxy* _agentBinaryProxy;
        std::shared_ptr<RawHandlerHostProxy> _handlerHostProxy;
        std::shared_ptr<MdmServerProxy> _mdmServerProxy;

        RawHandlerFactoryMap _rawFactoryMap;
        RawHandlerMap _rawHandlerMap;
    };

}}}}}
