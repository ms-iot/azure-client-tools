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
#include "..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "Plugins\AgentStub.h"
#include "Plugins\PluginBinaryProxy.h"
#include "Plugins\DirectPluginProxy.h"
#include "Handlers\ClearReportedCmdHandler.h"
#include "Handlers\DeviceSchemasHandler.h"
#include "..\AzureDeviceProvisioningClient\inc\DpsClientApi.h"

using namespace Microsoft::Azure::DeviceManagement::Common;
using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;
using namespace std::experimental::filesystem;
using namespace std::experimental::filesystem::v1;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

static const IOTHUB_CLIENT_TRANSPORT_PROVIDER IoTHubProtocol = AMQP_Protocol;

shared_ptr<AzureDMClient> AzureDMClient::_this;
mutex AzureDMClient::_lock;

AzureDMClient::AzureDMClient() :
    _deviceClientHandle(NULL),
    _moduleClientHandle(NULL),
    _sasTokenTimestampInTicks(0)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    _reportedSectionsSummary = make_shared<ReportedSummary>();
    _mdmServer = MdmServer::GetInstance();
}

std::shared_ptr<AzureDMClient> AzureDMClient::Create()
{
    LockGuard lk(&_lock);
    _this = make_shared<AzureDMClient>();
    AgentStub::SetMdmServer(_this->_mdmServer);
    AgentStub::SetHandlerHost(_this);
    return _this;
}

std::shared_ptr<AzureDMClient> AzureDMClient::GetInstance()
{
    LockGuard lk(&_lock);
    return _this;
}

void AzureDMClient::RegisterStaticRawHandler(
    std::shared_ptr<IRawHandler> handler)
{
    TRACELINEP(LoggingLevel::Verbose, "Registering: ", handler->GetId().c_str());

    handler->SetHandlerHost(_this);
    handler->SetMdmServer(_mdmServer);

    shared_ptr<RawHandlerInfo> rawHandlerInfo = make_shared<RawHandlerInfo>();
    rawHandlerInfo->_state = RawHandlerState::eInactive;
    rawHandlerInfo->_rawHandler = handler;

    _rawHandlerMap[handler->GetId()] = rawHandlerInfo;
}

void AzureDMClient::RegisterStaticRawHandlers()
{
    RegisterStaticRawHandler(make_shared<ClearReportedCmdHandler>(this));
    RegisterStaticRawHandler(make_shared<DeviceSchemasHandler>(&_rawHandlerMap));
}

void AzureDMClient::LogDMException(
    const DMException& ex,
    const std::string& message,
    const std::string& param)
{
    TRACELINEP(LoggingLevel::Error, message.c_str(), param.c_str());

    stringstream ss;
    ss << "Code: " << ex.Code() << " Message: " << ex.Message();
    TRACELINE(LoggingLevel::Error, ss.str().c_str());
}

void AzureDMClient::LogStdException(
    const std::exception& ex,
    const std::string& message,
    const std::string& param)
{
    TRACELINEP(LoggingLevel::Error, message.c_str(), param.c_str());

    stringstream ss;
    ss << " Message: " << ex.what();
    TRACELINE(LoggingLevel::Error, ss.str().c_str());
}

void AzureDMClient::RegisterDynamicHandler(
    shared_ptr<IPlugin> plugin,
    const HandlerInfo& handlerInfo)
{
    try
    {
        TRACELINEP(LoggingLevel::Verbose, "Registering Id   : ", handlerInfo.id.c_str());
        TRACELINEP(LoggingLevel::Verbose, "Registering Type : ", handlerInfo.type.c_str());

        if (handlerInfo.type == JsonHandlerTypeRaw)
        {
            shared_ptr<IRawHandler> handler = plugin->CreateRawHandler(handlerInfo.id);
            handler->SetMdmServer(_mdmServer);
            handler->SetHandlerHost(_this);

            shared_ptr<RawHandlerInfo> rawHandlerInfo = make_shared<RawHandlerInfo>();
            rawHandlerInfo->_state = RawHandlerState::eInactive;
            rawHandlerInfo->_rawHandler = handler;

            _rawHandlerMap[handlerInfo.id] = rawHandlerInfo;
        }
    }
    catch (const DMException& ex)
    {
        LogDMException(ex, "An error occured while registering: ", handlerInfo.id.c_str());
    }
    catch (const exception& ex)
    {
        LogStdException(ex, "An error occured while registering: ", handlerInfo.id.c_str());
    }
    catch (...)
    {
        TRACELINEP(LoggingLevel::Error, "An error occured while registering: ", handlerInfo.id.c_str());
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
            for (const HandlerInfo& handlerInfo : handlersInfo)
            {
                RegisterDynamicHandler(plugin, handlerInfo);
            }
            _pluginsMap[codeFileName] = plugin;
        }
        catch (const DMException& ex)
        {
            LogDMException(ex, "An error occured while processing: ", manifestFileName);
        }
        catch (const exception& ex)
        {
            LogStdException(ex, "An error occured while processing: ", manifestFileName);
        }
        catch (...)
        {
            TRACELINEP(LoggingLevel::Error, "An error occured while processing: ", manifestFileName.c_str());
        }
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

AzureDMClient::ConnectionStringInfo AzureDMClient::GetConnectionString() const
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

#if defined(_DEBUG)

    TRACELINE(LoggingLevel::Verbose, "Debug build... attempting to read connection string from configuration file...");

    string debugConnectionString = _serviceParameters->GetDebugConnectionString();
    if (debugConnectionString.size() != 0)
    {
        TRACELINE(LoggingLevel::Verbose, "Found connection string...");

        ConnectionStringInfo connectionStringInfo;
        connectionStringInfo.connectionString = debugConnectionString;
        connectionStringInfo.timestamp = GetTickCount64();

        return connectionStringInfo;
    }
#endif

    TRACELINE(LoggingLevel::Verbose, "Invoking TPM/DPS logic...");

    // Get DPS info
    auto uri = _serviceParameters->GetDpsUri();
    auto scopeId = _serviceParameters->GetDpsScopeId();

    // Get slot for device
    auto deviceSlot = _serviceParameters->GetDeviceSlotNumber();

    // Get dm slot/name info
    auto dmModuleId = _serviceParameters->GetDmModuleId();
    auto dmSlot = _serviceParameters->GetDmSlotNumber();    // default is 1

    // Get other module info: map<name, slot>
    auto otherModules = _serviceParameters->GetOtherModules();

    // Run DPS to get connection info and create modules as needed
    HRESULT hr = InitializeDpsClient();
    if (FAILED(hr))
    {
        TRACELINE(LoggingLevel::Error, L"InitializeDpsClient failed!");
        throw DMException(hr, "InitializeDpsClient failed!");
    }
    // Delete file when out of scope
    std::shared_ptr<void> deinitializeDpsClient(nullptr, [&](void*) {
        DeinitializeDpsClient();
    });

    hr = AzureDpsRegisterDevice(deviceSlot, uri, scopeId, dmSlot, dmModuleId, otherModules);
    if (FAILED(hr))
    {
        TRACELINE(LoggingLevel::Error, L"AzureDpsRegisterDevice failed!");
        throw DMException(hr, "AzureDpsRegisterDevice failed!");
    }

    // We record the timestamp 'before' the token generation so that we start renewal
    // before it expires.
    ULONGLONG timestamp = GetTickCount64();

    wstring wideDeviceConnectionString(L"");
    hr = TpmSupport::GetAzureConnectionString(dmSlot, _serviceParameters->GetSasTokenExpiryInterval(), wideDeviceConnectionString);
    if (FAILED(hr))
    {
        TRACELINE(LoggingLevel::Error, L"TpmSupport::GetAzureConnectionString failed!");
        throw DMException(hr, "TpmSupport::GetAzureConnectionString failed!");
    }

    string connectionString = WideToMultibyte(wideDeviceConnectionString.c_str());
    if (connectionString.size() == 0)
    {
        TRACELINE(LoggingLevel::Error, L"Unable to retrieve the connection string for IoT Hub!");
        throw DMException(ErrorCodeGeneric, "Unable to retrieve the connection string for IoT Hub!");
    }

    // Pack and return...
    ConnectionStringInfo connectionStringInfo;
    connectionStringInfo.timestamp = timestamp;
    connectionStringInfo.connectionString = connectionString;
    return connectionStringInfo;
}

void AzureDMClient::InitializeIoTHubConnection(shared_ptr<ServiceParameters> serviceParameters)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    _serviceParameters = serviceParameters;

    int initResult = platform_init();
    if (initResult != 0)
    {
        throw DMException(initResult, "Azure platform_init failed!");
    }

    // Get the connection string and its expiry...
    ConnectionStringInfo connectionStringInfo = GetConnectionString();
    _sasTokenTimestampInTicks = connectionStringInfo.timestamp;

    // Establish the connection...
    bool useDeviceTwin = serviceParameters->GetDeviceSlotNumber() == serviceParameters->GetDmSlotNumber();
    if (useDeviceTwin)
    {
        IOTHUB_DEVICE_CLIENT_LL_HANDLE clientHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionStringInfo.connectionString.c_str(), IoTHubProtocol);
        if (clientHandle == NULL)
        {
            throw DMException(ErrorCodeGeneric, "IoTHubClient_LL_CreateFromConnectionString failed!");
        }
        _deviceClientHandle = clientHandle;

        bool traceOn = true;
        IoTHubDeviceClient_LL_SetOption(_deviceClientHandle, OPTION_LOG_TRACE, &traceOn);

        IOTHUB_CLIENT_RESULT result = IoTHubDeviceClient_LL_SetDeviceMethodCallback(_deviceClientHandle, OnDirectMethodInvoked, this);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw DMException(result, "IoTHubDeviceClient_LL_SetDeviceMethodCallback failed!");
        }

        result = IoTHubDeviceClient_LL_SetDeviceTwinCallback(_deviceClientHandle, OnDeviceTwinReceived, this);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw DMException(result, "IoTHubDeviceClient_LL_SetDeviceTwinCallback failed!");
        }
    }
    else
    {
        IOTHUB_MODULE_CLIENT_LL_HANDLE clientHandle = IoTHubModuleClient_LL_CreateFromConnectionString(connectionStringInfo.connectionString.c_str(), IoTHubProtocol);
        if (clientHandle == NULL)
        {
            throw DMException(ErrorCodeGeneric, "IoTHubModuleClient_LL_CreateFromConnectionString failed!");
        }
        _moduleClientHandle = clientHandle;

        bool traceOn = true;
        IoTHubModuleClient_LL_SetOption(_moduleClientHandle, OPTION_LOG_TRACE, &traceOn);

        IOTHUB_CLIENT_RESULT result = IoTHubModuleClient_LL_SetModuleMethodCallback(_moduleClientHandle, OnDirectMethodInvoked, this);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw DMException(result, "IoTHubModuleClient_LL_SetModuleMethodCallback failed!");
        }

        result = IoTHubModuleClient_LL_SetModuleTwinCallback(_moduleClientHandle, OnDeviceTwinReceived, this);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw DMException(result, "IoTHubModuleClient_LL_SetModuleTwinCallback failed!");
        }
    }
}

void AzureDMClient::Deinitialize()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    if (_deviceClientHandle != NULL)
    {
        IoTHubDeviceClient_LL_Destroy(_deviceClientHandle);
        _deviceClientHandle = NULL;
    }
    if (_moduleClientHandle != NULL)
    {
        IoTHubModuleClient_LL_Destroy(_moduleClientHandle);
        _moduleClientHandle = NULL;
    }

    platform_deinit();
}

void AzureDMClient::SignalStart()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    for (auto& it : _rawHandlerMap)
    {
        TRACELINEP(LoggingLevel::Verbose, "Starting: ", it.first.c_str());

        const string& handleId = it.second->_rawHandler->GetId();

        bool active = false;

        try
        {
            Json::Value handlerConfig = BuildHandlerConfig(handleId);
            it.second->_rawHandler->Start(handlerConfig, active);
            it.second->_state = active ? RawHandlerState::eActive : RawHandlerState::eInactive;
            it.second->_rawHandler->OnConnectionStatusChanged(ConnectionStatus::eOnline);
        }
        catch (const DMException& ex)
        {
            LogDMException(ex, "An error occured while starting: ", handleId.c_str());
        }
        catch (const exception& ex)
        {
            LogStdException(ex, "An error occured while starting: ", handleId.c_str());
        }
        catch (...)
        {
            TRACELINEP(LoggingLevel::Error, "An error occured while starting: ", handleId.c_str());
        }
    }
}

void AzureDMClient::SignalStop()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    for (auto& it : _rawHandlerMap)
    {
        TRACELINEP(LoggingLevel::Verbose, "Stopping: ", it.first.c_str());

        it.second->_rawHandler->OnConnectionStatusChanged(ConnectionStatus::eOffline);
        it.second->_rawHandler->Stop();
    }
}

Json::Value AzureDMClient::BuildHandlerConfig(const string& handleId)
{
    Json::Value handlerConfig(Json::objectValue);
    handlerConfig[JsonTextLogFilesPath] = _serviceParameters->GetLogFilePath();

    Json::Value handlerParameters;
    if (_serviceParameters->GetHandlerParameters(handleId, handlerParameters))
    {
        vector<string> keys = handlerParameters.getMemberNames();
        for (const string& key : keys)
        {
            handlerConfig[key] = handlerParameters[key];
        }
    }

    return handlerConfig;
}

void AzureDMClient::Run(shared_ptr<ServiceParameters> serviceParameters, HANDLE stopEvent)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Static handlers...
    RegisterStaticRawHandlers();

    // Dynamic handlers...
    EnumeratePlugins(serviceParameters->GetPluginManifestPath());

    // InitializeIoTHubConnection...
    InitializeIoTHubConnection(serviceParameters);

    SignalStart();

    TRACELINE(LoggingLevel::Verbose, "Listening to IoT Hub notifications...");

    ULONGLONG gracePeriodInTicks = _serviceParameters->GetSasRenewalGracePeriod() * 1000;
    ULONGLONG expiryIntervalInTicks = _serviceParameters->GetSasTokenExpiryInterval() * 1000;
    ULONGLONG renewalStartInTicks = expiryIntervalInTicks - gracePeriodInTicks;

    // Calculate the new time after which we should attempt renewal.
    ULONGLONG renewalAbsoluteStartInTicks = _sasTokenTimestampInTicks + renewalStartInTicks;

    do
    {
        if (_deviceClientHandle != NULL)
        {
            IoTHubDeviceClient_LL_DoWork(_deviceClientHandle);
        }
        if (_moduleClientHandle != NULL)
        {
            IoTHubModuleClient_LL_DoWork(_moduleClientHandle);
        }
        ThreadAPI_Sleep(10);

        if (WaitForSingleObject(stopEvent, MessageLoopWait) == WAIT_OBJECT_0)
        {
            break;
        }

        if (GetTickCount64() > renewalAbsoluteStartInTicks)
        {
            TRACELINE(LoggingLevel::Verbose, "Connection is about to expire... renewing...");

            // Shutdown...
            SignalStop();
            Deinitialize();

            // Restart...
            InitializeIoTHubConnection(serviceParameters);
            SignalStart();

            // Calculate the new time after which we should attempt renewal.
            renewalAbsoluteStartInTicks = _sasTokenTimestampInTicks + renewalStartInTicks;
        }
    } while (true);

    // Clean-up...
    Deinitialize();

    TRACELINE(LoggingLevel::Verbose, "Azure thread is exiting...");
}

void AzureDMClient::OnReportedStateSent(int status_code, void* userContextCallback)
{
    AzureDMClient* This = static_cast<AzureDMClient*>(userContextCallback);
    This->OnReportedStateSent(status_code);
}

void AzureDMClient::OnDeviceTwinReceived(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t size, void* userContextCallback)
{
    AzureDMClient* This = static_cast<AzureDMClient*>(userContextCallback);
    This->OnDeviceTwinReceived(updateState, payload, size);
}

int AzureDMClient::OnDirectMethodInvoked(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback)
{
    AzureDMClient* This = static_cast<AzureDMClient*>(userContextCallback);
    return This->OnDirectMethodInvoked(methodName, payload, size, response, responseSize);
}

void AzureDMClient::OnReportedStateSent(int statusCode)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    TRACELINEP(LoggingLevel::Verbose, "OnReportedStateSent() - code: ", statusCode);
}

void AzureDMClient::Report(const Json::Value& partialReportedPropertiesObject)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    TRACELINE(LoggingLevel::Verbose, partialReportedPropertiesObject.toStyledString().c_str());

    Report(partialReportedPropertiesObject.toStyledString());
}

void AzureDMClient::Report(const string& partialReportedPropertiesString)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    TRACELINE(LoggingLevel::Verbose, partialReportedPropertiesString.c_str());

    if (_deviceClientHandle != NULL)
    {
        IOTHUB_CLIENT_RESULT result = IoTHubDeviceClient_LL_SendReportedState(_deviceClientHandle, (const unsigned char*)partialReportedPropertiesString.c_str(), partialReportedPropertiesString.size(), OnReportedStateSent, this);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw DMException(result, "IoTHubDeviceClient_LL_SendReportedState failed!");
        }
    }
    if (_moduleClientHandle != NULL)
    {
        IOTHUB_CLIENT_RESULT result = IoTHubModuleClient_LL_SendReportedState(_moduleClientHandle, (const unsigned char*)partialReportedPropertiesString.c_str(), partialReportedPropertiesString.size(), OnReportedStateSent, this);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw DMException(result, "IoTHubModuleClient_LL_SendReportedState failed!");
        }
    }
}

void AzureDMClient::Report(
    const std::string& handlerId,
    DeploymentStatus deploymentStatus,
    const std::string& resultJsonString)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Update the summary object
    _reportedSectionsSummary->SetSectionStatus(handlerId, deploymentStatus);

    // Combine the summary object and the result of this configuration into one transactions and submit them......
    Json::Value reportedProperties(Json::objectValue);
    reportedProperties[_reportedSectionsSummary->GetId()] = _reportedSectionsSummary->ToJsonObject();
    reportedProperties[handlerId] = JsonUtils::JsonObjectFromString(resultJsonString);
    Report(reportedProperties);
}

void AzureDMClient::Report(
    const std::string& handlerId,
    DeploymentStatus deploymentStatus,
    const Json::Value& value)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Update the summary object
    _reportedSectionsSummary->SetSectionStatus(handlerId, deploymentStatus);

    // Combine the summary object and the result of this configuration into one transactions and submit them......
    Json::Value reportedProperties(Json::objectValue);
    reportedProperties[_reportedSectionsSummary->GetId()] = _reportedSectionsSummary->ToJsonObject();
    reportedProperties[handlerId] = value;
    Report(reportedProperties);
}

void AzureDMClient::GetRegisteredHandlerNames(vector<string>& sectionNames)
{
    sectionNames.clear();

    for (RawHandlerMapType::const_iterator handlerIt = _rawHandlerMap.cbegin(); handlerIt != _rawHandlerMap.cend(); ++handlerIt)
    {
        sectionNames.emplace_back(handlerIt->first);
    }
}

void AzureDMClient::MarkPending(const Json::Value& desiredProperties)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    Json::Value reportedProperties(Json::objectValue);

    // Go through the "desired" children and mark them pending...
    vector<string> keys = desiredProperties.getMemberNames();
    for (const string& key : keys)
    {
        RawHandlerMapType::const_iterator handlerIt = _rawHandlerMap.find(key);
        if (handlerIt == _rawHandlerMap.cend())
        {
            continue;
        }

        Json::Value desiredConfig = desiredProperties[key];
        if (desiredConfig.isNull() || !desiredConfig.isObject())
        {
            continue;
        }

        const string& handlerId = handlerIt->second->_rawHandler->GetId();

        try
        {
            // noexcept
            _reportedSectionsSummary->SetSectionStatus(handlerId, DeploymentStatus::ePending);

            handlerIt->second->_rawHandler->SetDeploymentStatus(DeploymentStatus::ePending);
            reportedProperties[handlerId] = handlerIt->second->_rawHandler->GetDeploymentStatusJson();
        }
        catch (...)
        {
            // noexcept
            _reportedSectionsSummary->SetSectionStatus(handlerId, DeploymentStatus::eUnknown);

            // indicate something went wrong...
            reportedProperties[handlerId] = MetaData::DeploymentStatusToJsonObject(DeploymentStatus::eUnknown);
        }
    }

    reportedProperties[_reportedSectionsSummary->GetId()] = _reportedSectionsSummary->ToJsonObject();
    Report(reportedProperties);
}

InvokeResult AzureDMClient::InvokeHandler(
    const std::string& handlerId,
    const Json::Value& parametersJson)
{
    TRACELINEP(LoggingLevel::Verbose, "Looking for handler for: ", handlerId.c_str());

    InvokeResult invokeResult(false);

    RawHandlerMapType::const_iterator handlerIt = _rawHandlerMap.find(handlerId);
    if (handlerIt == _rawHandlerMap.cend())
    {
        TRACELINE(LoggingLevel::Verbose, "Handler not found.");
        return invokeResult;
    }
    TRACELINEP(LoggingLevel::Verbose, "Found handler for: ", handlerId.c_str());

    if (handlerIt->second->_state == RawHandlerState::eInactive)
    {
        TRACELINE(LoggingLevel::Verbose, "Handler is inactive.");
        return invokeResult;
    }

    // Invoke does not throw. Any errors should have been already reported by now.
    return handlerIt->second->_rawHandler->Invoke(parametersJson);
}

void AzureDMClient::OnDeviceTwinReceived(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* buffer, size_t size)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    TRACELINE(LoggingLevel::Verbose, "---- Desired Properties Start >> ----------------------------------------------------------------------");

    // Turn into a safe wide string...
    string payload((const char*)buffer, size);

    TRACELINEP(LoggingLevel::Verbose, "Received:", payload.c_str());

    // Parse the json and get the "desired" root...
    Json::Value root = JsonUtils::JsonObjectFromString(payload);

    Json::Value desiredProperties;

    // JsonObject^ desiredProperties;
    if (updateState == DEVICE_TWIN_UPDATE_COMPLETE)
    {
        TRACELINE(LoggingLevel::Verbose, "Entire device twin received.");

        if (JsonHelpers::HasKey(root, JsonDesired))
        {
            desiredProperties = root[JsonDesired];
        }
    }
    else
    {
        TRACELINE(LoggingLevel::Verbose, "Part of the device twin received.");
        desiredProperties = root;
    }

    // Go through the "desired" children and mark them pending...
    MarkPending(desiredProperties);

    // Go through the "desired" children and apply the desired changes...
    vector<string> keys = desiredProperties.getMemberNames();
    for (const string& key : keys)
    {
        TRACELINEP(LoggingLevel::Verbose, "Processing: ", key.c_str());
        InvokeResult result = InvokeHandler(key, desiredProperties[key]);
        assert(!result.present);    // present == true is only for direct methods.
    }

    TRACELINE(LoggingLevel::Verbose, "---- Desired Properties End << ------------------------------------------------------------------------");
}

int AzureDMClient::OnDirectMethodInvoked(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    TRACELINE(LoggingLevel::Verbose, "---- Direct Method Start >> ---------------------------------------------------------------------------");

    int returnCode = DirectMethodSuccess;
    string parameters((const char*)payload, size);

    TRACELINEP(LoggingLevel::Verbose, "Device Method Name:", methodName);
    TRACELINEP(LoggingLevel::Verbose, "Parameters        :", parameters.c_str());

    Json::Value parametersJson = JsonUtils::JsonObjectFromString(parameters);

    InvokeResult result = InvokeHandler(methodName, parametersJson);
    assert(result.present);

    TRACELINEP(LoggingLevel::Verbose, "Device Method Returned->payload: ", result.payload.c_str());
    TRACELINEP(LoggingLevel::Verbose, "Device Method Returned->code: ", result.code);

    // Prepare the return payload...
    *responseSize = result.payload.size();
    if ((*response = (unsigned char*)malloc(*responseSize)) == NULL)
    {
        returnCode = -1;
        result.payload = "Out of memory";  // This doesn't really go anywhere.
    }
    else
    {
        memcpy(*response, result.payload.c_str(), *responseSize);
        returnCode = result.code;
    }

    TRACELINE(LoggingLevel::Verbose, "---- Direct Method End << -----------------------------------------------------------------------------");

    return returnCode;
}

}}}}
