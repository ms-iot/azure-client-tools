// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <string>
#include <functional>

#include "iothub_module_client.h"
#include "iothub_device_client.h"

#include "../common/DMInterfaces.h"
#include "../common/plugins/PluginInterfaces.h"
#include "DeviceClientInterfaces.h"
#include "../ServiceParameters.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    enum RawHandlerState
    {
        eInactive,
        eActive
    };

    class RawHandlerInfo
    {
    public:
        RawHandlerInfo() :
            _state(eInactive)
        {}

        ~RawHandlerInfo();

        RawHandlerState _state;
        std::shared_ptr<DMCommon::IRawHandler> _rawHandler;
        std::vector<std::string> _dependencies;
    };

    typedef std::map<std::string, RawHandlerInfo> RawHandlerMapType;

    class AzureRawHost : public IDeviceClient, public DMCommon::IRawHandlerHost, public IRawHandlerRouter
    {
        AzureRawHost();

    public:
        static std::shared_ptr<AzureRawHost> Create();
        ~AzureRawHost();

        void SetServiceParameters(
            std::shared_ptr<ServiceParameters> serviceParameters);

        void RegisterStaticHandlers();

        void SetMdmServer(
            std::shared_ptr<DMCommon::IMdmServer> mdmServer);

        std::shared_ptr<DMCommon::IMdmServer> GetMdmServer() const;

        void SetHardwareIds(
            std::shared_ptr<std::map<std::string, std::vector<std::string>>> handlersHardwareIds);

        void InitializeDeviceConnection(
            IOTHUB_DEVICE_CLIENT_HANDLE clientHandle);

        void InitializeModuleConnection(
            IOTHUB_MODULE_CLIENT_HANDLE moduleHandle);

        void EnableIoTHubNotifications(
            bool enable);

        void RegisterDynamicHandler(
            std::shared_ptr<DMCommon::IPlugin> plugin,
            const DMCommon::HandlerInfo& handlerInfo);

        void UnregisterDynamicHandler(
            const std::string& handlerId);

        void HandlerInvokeSequence();

        InvokeHandlerResult InvokeHandler(
            const std::string& handlerId,
            const Json::Value& parametersJson);

        void Report(
            const std::string& handlerId,
            DMCommon::DeploymentStatus deploymentStatus,
            const Json::Value& value);

        void SendEvent(
            const std::string& handlerId,
            const std::string& eventName,
            const Json::Value& messageData);

        void SignalStart();

        Json::Value BuildHandlerConfig(
            const std::string& handlerId);

        void SignalStop();

        void SignalConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        void Destroy();

    private:

        void RegisterStaticHandler(
            std::shared_ptr<DMCommon::IRawHandler> handler);

        void ReportAll(
            const Json::Value& partialReportedPropertiesObject);

        void ReportAll(
            const std::string& partialReportedPropertiesString);

        void GetHandlerIds(
            std::vector<std::string>& handlerIds);

        static void OnSendEventSent(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
        static void OnReportedStateSent(int statusCode, void* userContextCallback);
        static void OnDeviceTwinReceived(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad, size_t size, void* userContextCallback);
        static int OnDirectMethodInvoked(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback);

        void OnSendEventSent(IOTHUB_CLIENT_CONFIRMATION_RESULT result);
        void OnReportedStateSent(int statusCode);
        void OnDeviceTwinReceived(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad, size_t size);
        int OnDirectMethodInvoked(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize);

        void MarkStatus(const Json::Value& desiredProperties, DMCommon::DeploymentStatus status);

        // Data

        static std::recursive_mutex _lock;
        static std::shared_ptr<AzureRawHost> _this;
        static std::shared_ptr<DMCommon::IMdmServer> _mdmServer;

        std::shared_ptr<ServiceParameters> _serviceParameters;
        IOTHUB_DEVICE_CLIENT_HANDLE _deviceClientHandle;
        IOTHUB_MODULE_CLIENT_HANDLE _moduleClientHandle;

        std::shared_ptr<Microsoft::Azure::DeviceManagement::Common::ReportedSummary> _reportedSectionsSummary;

        std::shared_ptr<std::map<std::string, std::vector<std::string>>> _handlersHardwareIds;

        // Handlers
        RawHandlerMapType _rawHandlerMap;

        std::vector<std::string> _rawHandlerSequence;
    };

}}}}