// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <assert.h>
#include <wrl\client.h>

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_client.h"
#include "iothub_device_client.h"
#include "iothub_module_client.h"
#include "iothub_client_options.h"
#include "iothub_message.h"
#include "iothubtransportamqp.h"

#include "AzureDMClient.h"
#include "DMConstants.h"
#include "Mdm\MdmServer.h"
#include "PluginManifest.h"
#include "../common/plugins/PluginConstants.h"
#include "Plugins\AgentStub.h"
#include "Plugins\PluginBinaryProxy.h"
#include "Plugins\DirectPluginProxy.h"
#include "../../AzureDeviceProvisioningClient/inc/DpsClientApi.h"

#ifdef USE_AZURE_DM_BRIDGE
#include "../uwp-bridge-lib/DMBridge/DMBridgeServer.h"
#endif // USE_AZURE_DM_BRIDGE

using namespace Microsoft::Azure::DeviceManagement::Common;
using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;
using namespace std::experimental::filesystem;
using namespace std::experimental::filesystem::v1;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

static const IOTHUB_CLIENT_TRANSPORT_PROVIDER IoTHubProtocol = AMQP_Protocol;

shared_ptr<AzureDMClient> AzureDMClient::_this;
recursive_mutex AzureDMClient::_lock;

AzureDMClient::AzureDMClient() :
    _deviceClientHandle(NULL),
    _moduleClientHandle(NULL),
    _sasTokenTimestampInTicks(0),
    _azureInterfaceType(AzureInterfaceType::eRaw)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
}

std::shared_ptr<AzureDMClient> AzureDMClient::Create()
{
    LockGuard lk(&_lock);
    _this = make_shared<AzureDMClient>();
    return _this;
}

std::shared_ptr<AzureDMClient> AzureDMClient::GetInstance()
{
    LockGuard lk(&_lock);
    return _this;
}

void AzureDMClient::RegisterDynamicHandler(
    shared_ptr<IPlugin> plugin,
    const HandlerInfo& handlerInfo)
{
    try
    {
        TRACELINEP(LoggingLevel::Verbose, "Registering Id   : ", handlerInfo.id.c_str());
        TRACELINEP(LoggingLevel::Verbose, "Registering Type : ", handlerInfo.type.c_str());

        _activeHandlerHost->RegisterDynamicHandler(plugin, handlerInfo);
    }
    catch (const DMException& ex)
    {
        LogDMException(ex, "An error occured while registering: ", handlerInfo.id.c_str());
    }
    catch (const exception& ex)
    {
        LogStdException(ex, "An error occured while registering: ", handlerInfo.id.c_str());
    }
}

void AzureDMClient::EnumeratePlugins(const string& manifestsPath)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    TRACELINEP(LoggingLevel::Verbose, "Enumerating plugin manifests from: ", manifestsPath.c_str());

    // ToDo: clean-up - use C++... or handle error.
    // Make sure the directory exists.
    CreateDirectoryA(manifestsPath.c_str(), NULL);

    vector<PluginManifest> pluginManifests;
    for (auto & pathIt : directory_iterator(manifestsPath))
    {
        if (pathIt.status().type() != file_type::regular)
        {
            continue;
        }

        string manifestFileName = Utils::WideToMultibyte(pathIt.path().c_str());

        TRACELINEP(LoggingLevel::Verbose, "Found Manifest: ", manifestFileName.c_str());

        try
        {
            // Parse the manifest...
            PluginManifest manifest;
            manifest.FromJsonFile(manifestFileName);

            string manifestAgentPluginProtocolVersion = manifest.GetAgentPluginProtocolVersion();

            //checking if the agent-plugin protocol major version of plugin is same as that of the device agent
            if (MajorVersionCompare(AgentPluginProtocolVersion, manifestAgentPluginProtocolVersion) == 0)
            {
                // Load the plug-in binary...
                string codeFileName = manifest.GetCodeFileName();
                TRACELINEP(LoggingLevel::Verbose, "Loading: ", codeFileName.c_str());

                shared_ptr<IPlugin> plugin;

                if (manifest.IsDirect())
                {
                    plugin = make_shared<DirectPluginProxy>(codeFileName);
                }
                else
                {
                    bool outOfProc = manifest.IsOutOfProc();
                    TRACELINEP(LoggingLevel::Verbose, "Is Out of proc: ", outOfProc);

                    long keepAliveTime = INFINITE;
                    if (outOfProc)
                    {
                        keepAliveTime = manifest.GetKeepAliveTime();
                        TRACELINEP(LoggingLevel::Verbose, "Keep alive time: ", keepAliveTime);
                    }
                    plugin = make_shared<PluginProxy>(codeFileName, outOfProc, keepAliveTime);
                }

                plugin->Load();

                // Get the handlers information...
                vector<HandlerInfo> handlersInfo = plugin->GetHandlersInfo();
                auto handlersInfoFromManifest = manifest.GetHandlers();

                for (auto& handler : handlersInfo)
                {
                    if (handlersInfoFromManifest[handler.id])
                    {
                        handler.dependencies = handlersInfoFromManifest[handler.id]->GetDependencies();
                    }
                }

                for (const HandlerInfo& handlerInfo : handlersInfo)
                {
                    RegisterDynamicHandler(plugin, handlerInfo);
                }
                _pluginsMap[codeFileName] = plugin;
            }
            else
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_PROTOCOL_VERSION, "Plugin built against a different version of the device agent.");
            }
        }
        catch (const DMException& ex)
        {
            LogDMException(ex, "An error occured while processing: ", manifestFileName);
        }
        catch (const exception& ex)
        {
            LogStdException(ex, "An error occured while processing: ", manifestFileName);
        }
    }
}

void AzureDMClient::HandlerInvokeSequence()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    try
    {
        _activeHandlerHost->HandlerInvokeSequence();
    }
    catch (const DMException& ex)
    {
        LogDMException(ex, "An error occured while sequencing handlers.", "");
    }
}

AzureDMClient::~AzureDMClient()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    std::map<std::string, std::shared_ptr<DMCommon::IPlugin>>::iterator it;
    for (it = _pluginsMap.begin(); it != _pluginsMap.end(); it++)
    {
        TRACELINEP(LoggingLevel::Verbose, "Unloading plugin: ", it->first.c_str());
        it->second->Unload();
    }

    Deinitialize();
}

void AzureDMClient::ProvisionDevice(std::shared_ptr<ServiceParameters> serviceParameters)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Get DPS info
    auto uri = serviceParameters->GetDpsUri();

#ifdef USE_AZURE_DM_BRIDGE
    TRACELINE(LoggingLevel::Verbose, "Waiting for Dps Scope Id to be set by the DM Bridge...");
    serviceParameters->WaitDpsScopeId();
#endif

    auto scopeId = serviceParameters->GetDpsScopeId();

    // Get slot for device
    auto dmSlot = serviceParameters->GetDmSlotNumber();
    auto deviceSlot = serviceParameters->GetDeviceSlotNumber();

    // Get the DM module id
    auto dmModuleId = serviceParameters->GetDmModuleId();

    // Get other module info: map<name, slot>
    auto otherModules = serviceParameters->GetOtherModules();

    // Run DPS to get connection info and create modules as needed
    HRESULT hr = InitializeDpsClient();
    if (FAILED(hr))
    {
        TRACELINE(LoggingLevel::Error, L"InitializeDpsClient failed!");
        throw DMException(DMSubsystem::DPS, hr, "InitializeDpsClient failed!");
    }
    // Delete file when out of scope
    std::shared_ptr<void> deinitializeDpsClient(nullptr, [&](void*) {
        DeinitializeDpsClient();
    });

    hr = AzureDpsRegisterDevice(deviceSlot, uri, scopeId, dmSlot, dmModuleId, otherModules);
    if (FAILED(hr))
    {
        TRACELINE(LoggingLevel::Error, L"AzureDpsRegisterDevice failed!");
        throw DMException(DMSubsystem::DPS, hr, "AzureDpsRegisterDevice failed!");
    }
}

string AzureDMClient::GetConnectionString(
    unsigned int dmSlot,
    unsigned int sasTokenExpiryInterval)
{
    HRESULT hr = TpmSupport::InitializeTpm();
    if (FAILED(hr))
    {
        TRACELINEP(LoggingLevel::Verbose, "Failed to initialize TPM subsystem.", hr);
        return "";
    }

    shared_ptr<void> deinitializeDpsClient(nullptr, [&](void*) {
        TpmSupport::DeinitializeTpm();
    });

    wstring wideDeviceConnectionString(L"");
    hr = TpmSupport::GetAzureConnectionString(dmSlot, sasTokenExpiryInterval, wideDeviceConnectionString);
    if (FAILED(hr))
    {
        TRACELINE(LoggingLevel::Error, L"TpmSupport::GetAzureConnectionString failed!");
        throw DMException(DMSubsystem::Limpet, hr, "TpmSupport::GetAzureConnectionString failed!");
    }

    return WideToMultibyte(wideDeviceConnectionString.c_str());
}


AzureDMClient::ConnectionStringInfo AzureDMClient::GetConnectionString() const
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    TRACELINE(LoggingLevel::Verbose, "Attempting to read the connection string from configuration file...");

    string connectionString = _serviceParameters->GetConnectionString();
    if (connectionString.size() != 0)
    {
        TRACELINE(LoggingLevel::Verbose, "Found connection string...");
        return ConnectionStringInfo(connectionString, GetTickCount64());
    }

    TRACELINE(LoggingLevel::Verbose, "Attempting to read the connection string from the TPM...");

    ULONGLONG timestamp = 0;
    bool triedDps = false;

    while (true)
    {
        // We record the timestamp 'before' the token generation so that we start renewal before it expires.
        timestamp = GetTickCount64();

        try
        {
            connectionString = GetConnectionString(_serviceParameters->GetDmSlotNumber(), _serviceParameters->GetSasTokenExpiryInterval());
            if (connectionString.size() != 0)
            {
                TRACELINE(LoggingLevel::Verbose, "Connection string retrieved successfully from the TPM...");
                break;
            }
            TRACELINE(LoggingLevel::Verbose, "Connection string could not be found in the TPM...");
        }
        catch (const DMException& ex)
        {
            TRACELINEP(LoggingLevel::Warning, "Could not retrieve the connection string from the TPM. Error: ", ex.DisplayMessage().c_str());
        }
        catch (const exception& ex)
        {
            TRACELINEP(LoggingLevel::Warning, "Could not retrieve the connection string from the TPM. Error: ", ex.what());
        }

        if (_serviceParameters->IsDpsEnabled())
        {
            // At this point, we were not able to retrieve the connection string...
            if (triedDps)
            {
                // If we have tried DPS already, there is nothing else to try.
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_DPS_CLIENT_FAILED, "Unable to retrieve the connection string for IoT Hub!");
            }

            ProvisionDevice(_serviceParameters);

            triedDps = true;
        }
        else
        {
            // Just wait before trying again...
            // The expectation is that the connection string will be written by another provisioning service/process.

            TRACELINE(LoggingLevel::Verbose, "Waiting for the TPM to be provisioned...");
            ::Sleep(10 * 1000); // Wait 10 seconds.
        }
    }

    return ConnectionStringInfo(connectionString, timestamp);
}

void AzureDMClient::InitializeIoTHubConnection()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    int initResult = platform_init();
    if (initResult != 0)
    {
        throw DMException(DMSubsystem::IotHub, initResult, "Azure platform_init failed!");
    }

    // Get the connection string and its expiry...
    ConnectionStringInfo connectionStringInfo = GetConnectionString();
    _sasTokenTimestampInTicks = connectionStringInfo.timestamp;

    // Establish the connection...
    bool useDeviceTwin = _serviceParameters->GetDeviceSlotNumber() == _serviceParameters->GetDmSlotNumber();
    if (useDeviceTwin)
    {
        TRACELINE(LoggingLevel::Verbose, "Using device twin...");

        IOTHUB_DEVICE_CLIENT_HANDLE clientHandle = IoTHubDeviceClient_CreateFromConnectionString(connectionStringInfo.connectionString.c_str(), IoTHubProtocol);
        if (clientHandle == NULL)
        {
            throw DMException(DMSubsystem::IotHub, DM_ERROR_OPERATION_FAILED, "IoTHubDeviceClient_CreateFromConnectionString failed!");
        }
        _deviceClientHandle = clientHandle;

        bool traceOn = true;
        IoTHubDeviceClient_SetOption(_deviceClientHandle, OPTION_LOG_TRACE, &traceOn);

        _activeHandlerHost->InitializeDeviceConnection(_deviceClientHandle);
    }
    else
    {
        TRACELINE(LoggingLevel::Verbose, "Using module twin...");

        IOTHUB_MODULE_CLIENT_HANDLE clientHandle = IoTHubModuleClient_CreateFromConnectionString(connectionStringInfo.connectionString.c_str(), IoTHubProtocol);
        if (clientHandle == NULL)
        {
            throw DMException(DMSubsystem::IotHub, DM_ERROR_OPERATION_FAILED, "IoTHubModuleClient_CreateFromConnectionString failed!");
        }
        _moduleClientHandle = clientHandle;

        bool traceOn = true;
        IoTHubModuleClient_SetOption(_moduleClientHandle, OPTION_LOG_TRACE, &traceOn);

        if (_azureInterfaceType == AzureInterfaceType::eRaw)
        {
            _activeHandlerHost->InitializeModuleConnection(_moduleClientHandle);
        }
    }
}

void AzureDMClient::Deinitialize()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    _activeHandlerHost->Destroy();

    platform_deinit();
}

void AzureDMClient::Run(shared_ptr<ServiceParameters> serviceParameters, HANDLE stopEvent)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    _serviceParameters = serviceParameters;

    string azureInterfaceType = _serviceParameters->GetAzureInterfaceType();

    if (azureInterfaceType == JsonAzureInterfaceTypeRaw)
    {
        _azureInterfaceType = AzureInterfaceType::eRaw;
        _activeHandlerHost = _rawHandlerHost = AzureRawHost::GetInstance();
        AgentStub::SetRawHandlerHost(_this->_rawHandlerHost);
    }

    _mdmServer = MdmServer::GetInstance();
    AgentStub::SetMdmServer(_this->_mdmServer);

    _activeHandlerHost->SetServiceParameters(_serviceParameters);
    _activeHandlerHost->SetMdmServer(_mdmServer);

#ifdef USE_AZURE_DM_BRIDGE
    // Start bridge...
    StartAzureDeviceManagementUwpBridge();
#endif // USE_AZURE_DM_BRIDGE

    // Static handlers...
    _activeHandlerHost->RegisterStaticHandlers();

    // Dynamic handlers...
    EnumeratePlugins(serviceParameters->GetPluginManifestPath());

    _activeHandlerHost->SignalStart();

    ULONGLONG gracePeriodInTicks = _serviceParameters->GetSasRenewalGracePeriod() * 1000;
    ULONGLONG expiryIntervalInTicks = _serviceParameters->GetSasTokenExpiryInterval() * 1000;
    ULONGLONG renewalStartInTicks = expiryIntervalInTicks - gracePeriodInTicks;
    ULONGLONG renewalAbsoluteStartInTicks = 0;

    HandlerInvokeSequence();

    bool firstTime = true;
    bool connected = false;
    do
    {

        if (WaitForSingleObject(stopEvent, MessageLoopWait) == WAIT_OBJECT_0)
        {
            TRACELINE(LoggingLevel::Verbose, "Received signal to exit...");
            break;
        }

        bool renew = false;
        
        if (!firstTime)
        {
            renew = GetTickCount64() > renewalAbsoluteStartInTicks;
            if (renew)
            {
                TRACELINE(LoggingLevel::Verbose, "---- ---- Renewing SAS token ---- ----");
            }
        }

        if (firstTime || renew)
        {
            if (connected)
            {
                TRACELINE(LoggingLevel::Verbose, "---- ---- Disconnecting ---- ----");
                // Shutdown...
                _activeHandlerHost->EnableIoTHubNotifications(false);
                _activeHandlerHost->SignalConnectionStatusChanged(ConnectionStatus::eOffline);
                Deinitialize();
                connected = false;
            }

            TRACELINE(LoggingLevel::Verbose, "---- ---- Connecting ---- ----");
            // Restart...
            InitializeIoTHubConnection();
            _activeHandlerHost->EnableIoTHubNotifications(true);
            _activeHandlerHost->SignalConnectionStatusChanged(ConnectionStatus::eOnline);

            connected = true;
            firstTime = false;

            // Calculate the new time after which we should attempt renewal.
            renewalAbsoluteStartInTicks = _sasTokenTimestampInTicks + renewalStartInTicks;
        }

        if (connected)
        {
            ThreadAPI_Sleep(1000);  // 1 second
        }

    } while (true);

    _activeHandlerHost->EnableIoTHubNotifications(false);
    _activeHandlerHost->SignalConnectionStatusChanged(ConnectionStatus::eOffline);
    _activeHandlerHost->SignalStop();
    Deinitialize();

    TRACELINE(LoggingLevel::Verbose, "Azure thread is exiting...");
}

#ifdef USE_AZURE_DM_BRIDGE

void AzureDMClient::StartAzureDeviceManagementUwpBridge()
{
    shared_ptr<IRawHandlerRouter> rawHandlerRouter = dynamic_pointer_cast<IRawHandlerRouter>(_activeHandlerHost);
    if (rawHandlerRouter == nullptr)
    {
        TRACELINE(LoggingLevel::Error, "Failed to initialize UWP bridge. Current active client does not support IRawHandlerRouter");
        return;
    }

    _localDMClient = shared_ptr<LocalDMClient>(new LocalDMClient(_serviceParameters, rawHandlerRouter));

    std::thread uwpBridgeThread([&] {

        TRACELINE(LoggingLevel::Verbose, "Directly starting DMBridgeServer");
        DMBridgeServer::Setup(_localDMClient);
        DMBridgeServer::Listen();
        TRACELINE(LoggingLevel::Verbose, "DMBridgeServer finished");
    });
    uwpBridgeThread.detach();
}

#endif // USE_AZURE_DM_BRIDGE

}}}}