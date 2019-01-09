// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include "ServiceParameters.h"
#include "IoTHub\IoTHubInterfaces.h"
#include "iothub_module_client.h"
#include "iothub_device_client.h"
#include "Plugins\PluginProxy.h"
#include "Mdm\MdmServer.h"
#include "AzureRawHost.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class AzureDMClient : public IDeviceTwin, public DMCommon::IRawHandlerHost
    {
        struct ConnectionStringInfo
        {
            std::string connectionString;
            ULONGLONG timestamp;
        };

        AzureDMClient();
    public:
        static std::shared_ptr<AzureDMClient> Create();
        static std::shared_ptr<AzureDMClient> GetInstance();

        ~AzureDMClient();

        void Run(
            std::shared_ptr<ServiceParameters> serviceParameters, HANDLE stopEvent);

        void Report(
            const std::string& handlerId,
            DMCommon::DeploymentStatus deploymentStatus,
            const std::string& resultJsonString);

        void Report(
            const std::string& id,
            DMCommon::DeploymentStatus deploymentStatus,
            const Json::Value& value);


    private:
        friend std::_Ref_count_obj<AzureDMClient>;

        void InitializeIoTHubConnection(
            std::shared_ptr<ServiceParameters> serviceParameters);

        Json::Value BuildHandlerConfig(
            const std::string& handleId);

        void SignalStart();

        void SignalStop();

        void Deinitialize();

        ConnectionStringInfo GetConnectionString() const;

        void RegisterStaticRawHandler(
            std::shared_ptr<DMCommon::IRawHandler> handler);

        void RegisterStaticRawHandlers();

        void RegisterDynamicHandler(
            std::shared_ptr<DMCommon::IPlugin> plugin,
            const DMCommon::HandlerInfo& handlerInfo);

        void EnumeratePlugins(
            const std::string& manifestsPath);

        static void OnReportedStateSent(int statusCode, void* userContextCallback);
        static void OnDeviceTwinReceived(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad, size_t size, void* userContextCallback);
        static int OnDirectMethodInvoked(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback);

        void OnReportedStateSent(int statusCode);
        void OnDeviceTwinReceived(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad, size_t size);
        int OnDirectMethodInvoked(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize);

        DMCommon::InvokeResult InvokeHandler(
            const std::string& handlerId,
            const Json::Value& parametersJson);

        // IDeviceTwin
        void Report(
            const Json::Value& partialReportedPropertiesObject);

        void Report(
            const std::string& partialReportedPropertiesString);

        void GetRegisteredHandlerNames(
            std::vector<std::string>& sectionNames);

        void MarkPending(
            const Json::Value& desiredProperties);

        static void LogDMException(
            const DMUtils::DMException& ex,
            const std::string& message,
            const std::string& param);

        static void LogStdException(
            const std::exception& ex,
            const std::string& message,
            const std::string& param);

        // Data Members
        static std::shared_ptr<AzureDMClient> _this;
        static std::mutex _lock;

        ULONGLONG _sasTokenTimestampInTicks;

        std::shared_ptr<ServiceParameters> _serviceParameters;
        IOTHUB_DEVICE_CLIENT_LL_HANDLE _deviceClientHandle;
        IOTHUB_MODULE_CLIENT_LL_HANDLE _moduleClientHandle;

        std::shared_ptr<Microsoft::Azure::DeviceManagement::Common::ReportedSummary> _reportedSectionsSummary;
        std::shared_ptr<DMCommon::IMdmServer> _mdmServer;

        // Plug-ins
        std::map<std::string, std::shared_ptr<DMCommon::IPlugin>> _pluginsMap;

        // Handlers
        RawHandlerMapType _rawHandlerMap;
    };

}}}}
