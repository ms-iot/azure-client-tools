// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include "ServiceParameters.h"
#include "iothub_module_client.h"
#include "iothub_device_client.h"
#include "Plugins\PluginProxy.h"
#include "Mdm\MdmServer.h"
#include "Raw\AzureRawHost.h"

#ifdef USE_AZURE_DM_BRIDGE
#include "LocalDMClient.h"
#endif // USE_AZURE_DM_BRIDGE

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class AzureDMClient
    {
        enum AzureInterfaceType
        {
            eRaw = 0x01
        };

        struct ConnectionStringInfo
        {
            std::string connectionString;
            ULONGLONG timestamp;

            ConnectionStringInfo(
                std::string connectionString_,
                ULONGLONG timestamp_) :
                connectionString(connectionString_),
                timestamp(timestamp_)
            {}
        };

        AzureDMClient();

    public:
        static std::shared_ptr<AzureDMClient> Create();
        static std::shared_ptr<AzureDMClient> GetInstance();

        ~AzureDMClient();

        void Run(
            std::shared_ptr<ServiceParameters> serviceParameters, HANDLE stopEvent);

    private:
        friend std::_Ref_count_obj<AzureDMClient>;

#ifdef USE_AZURE_DM_BRIDGE

        std::shared_ptr<LocalDMClient> _localDMClient;

        void StartAzureDeviceManagementUwpBridge();

#endif // USE_AZURE_DM_BRIDGE

        void InitializeIoTHubConnection();

        void Deinitialize();

        ConnectionStringInfo GetConnectionString() const;

        static std::string GetConnectionString(
            unsigned int dmSlot,
            unsigned int sasTokenExpiryInterval);

        static void ProvisionDevice(
            std::shared_ptr<ServiceParameters> serviceParameters);

        void RegisterDynamicHandler(
            std::shared_ptr<DMCommon::IPlugin> plugin,
            const DMCommon::HandlerInfo& handlerInfo);

        void EnumeratePlugins(
            const std::string& manifestsPath);

        void HandlerInvokeSequence();

      // Data Members
        static std::shared_ptr<AzureDMClient> _this;
        static std::recursive_mutex _lock;

        unsigned int _azureInterfaceType;

        ULONGLONG _sasTokenTimestampInTicks;

        std::shared_ptr<ServiceParameters> _serviceParameters;
        IOTHUB_DEVICE_CLIENT_HANDLE _deviceClientHandle;
        IOTHUB_MODULE_CLIENT_HANDLE _moduleClientHandle;

        std::shared_ptr<DMCommon::IMdmServer> _mdmServer;
        std::shared_ptr<AzureRawHost> _rawHandlerHost;
        std::shared_ptr<IDeviceClient> _activeHandlerHost;

        // Plug-ins
        std::map<std::string, std::shared_ptr<DMCommon::IPlugin>> _pluginsMap;
    };

}}}}