// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ServiceParameters.h"
#include "iothub_module_client.h"
#include "iothub_device_client.h"
#include "../common/DMInterfaces.h"
#include "../common/plugins/PluginInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class IDeviceClient
    {
    public:

        virtual void SetServiceParameters(
            std::shared_ptr<ServiceParameters> serviceParameters) = 0;

        virtual void RegisterStaticHandlers() = 0;

        virtual void RegisterDynamicHandler(
            std::shared_ptr<DMCommon::IPlugin> plugin,
            const DMCommon::HandlerInfo& handlerInfo) = 0;

        virtual void HandlerInvokeSequence() = 0;

        virtual void SetMdmServer(
            std::shared_ptr<DMCommon::IMdmServer> mdmServer) = 0;

        virtual void SetHardwareIds(
            std::shared_ptr<std::map<std::string, std::vector<std::string>>> handlersHardwareIds) = 0;

        virtual void InitializeDeviceConnection(
            IOTHUB_DEVICE_CLIENT_HANDLE clientHandle) = 0;

        virtual void InitializeModuleConnection(
            IOTHUB_MODULE_CLIENT_HANDLE moduleHandle) = 0;

        virtual void EnableIoTHubNotifications(
            bool enable) = 0;

        virtual void SignalStart() = 0;

        virtual void SignalStop() = 0;

        virtual void SignalConnectionStatusChanged(
            DMCommon::ConnectionStatus status) = 0;

        virtual void Destroy() = 0;

        virtual ~IDeviceClient() {}
    };

    struct InvokeHandlerResult
    {
        bool found;
        bool active;
        DMCommon::InvokeResult invokeResult;

        InvokeHandlerResult() :
            found(false),
            active(false)
        {}
    };

    class IRawHandlerRouter
    {
    public:

        virtual InvokeHandlerResult InvokeHandler(
            const std::string& handlerId,
            const Json::Value& parametersJson) = 0;

        virtual ~IRawHandlerRouter() {}
    };

}}}}