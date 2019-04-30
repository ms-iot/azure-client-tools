// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_client.h"
#include "iothub_device_client.h"
#include "iothub_module_client.h"
#include "iothub_client_options.h"
#include "iothub_device_client.h"
#include "iothub_message.h"
#include "iothubtransportamqp.h"

#include "AzureRawHost.h"

#include "..\Handlers\ClearReportedCmdHandler.h"
#include "..\Handlers\DeviceSchemasHandler.h"
#include "../../common/plugins/PluginInterfaces.h"

using namespace DMUtils;
using namespace DMCommon;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    shared_ptr<AzureRawHost> AzureRawHost::_this;
    shared_ptr<IMdmServer> AzureRawHost::_mdmServer;
    recursive_mutex AzureRawHost::_lock;

    AzureRawHost::AzureRawHost() :
        _deviceClientHandle(NULL),
        _moduleClientHandle(NULL)
    {
        _reportedSectionsSummary = make_shared<ReportedSummary>();
    }

    std::shared_ptr<AzureRawHost> AzureRawHost::GetInstance()
    {
        LockGuard lk(&_lock);

        if (_this == nullptr)
        {
            _this = shared_ptr<AzureRawHost>(new AzureRawHost());
        }

        return _this;
    }

    void AzureRawHost::SetServiceParameters(
        std::shared_ptr<ServiceParameters> serviceParameters)
    {
        _serviceParameters = serviceParameters;
    }

    void AzureRawHost::SetMdmServer(
        shared_ptr<IMdmServer> mdmServer)
    {
        _mdmServer = mdmServer;
    }

    shared_ptr<IMdmServer> AzureRawHost::GetMdmServer() const
    {
        return _mdmServer;
    }
    
    Json::Value AzureRawHost::BuildHandlerConfig(
        const string& handlerId)
    {
        Json::Value handlerConfig(Json::objectValue);
        handlerConfig[JsonTextLogFilesPath] = _serviceParameters->GetLogFilePath();
        handlerConfig[JsonPluginsDataPath] = _serviceParameters->GetPluginsDataPath();

        Json::Value handlerParameters;
        if (_serviceParameters->GetHandlerParameters(handlerId, handlerParameters))
        {
            vector<string> keys = handlerParameters.getMemberNames();
            for (const string& key : keys)
            {
                handlerConfig[key] = handlerParameters[key];
            }
        }

        return handlerConfig;
    }

    void AzureRawHost::InitializeDeviceConnection(
          IOTHUB_DEVICE_CLIENT_HANDLE clientHandle)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        _deviceClientHandle = clientHandle;
    }

    void AzureRawHost::InitializeModuleConnection(
        IOTHUB_MODULE_CLIENT_HANDLE moduleHandle)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        _moduleClientHandle = moduleHandle;
    }

    void AzureRawHost::EnableIoTHubNotifications(
        bool enable)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        void* ctx = nullptr;
        IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC directMethodCallback = nullptr;
        IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK twinCallback = nullptr;

        if (enable)
        {
            ctx = this;
            directMethodCallback = OnDirectMethodInvoked;
            twinCallback = OnDeviceTwinReceived;
        }

        if (_deviceClientHandle)
        {
            IOTHUB_CLIENT_RESULT result = IoTHubDeviceClient_SetDeviceMethodCallback(_deviceClientHandle, directMethodCallback, ctx);
            if (result != IOTHUB_CLIENT_OK)
            {
                throw DMException(DMSubsystem::IotHub, result, "IoTHubDeviceClient_SetDeviceMethodCallback failed!");
            }

            result = IoTHubDeviceClient_SetDeviceTwinCallback(_deviceClientHandle, twinCallback, ctx);
            if (result != IOTHUB_CLIENT_OK)
            {
                throw DMException(DMSubsystem::IotHub, result, "IoTHubDeviceClient_SetDeviceTwinCallback failed!");
            }
        }
        else if (_moduleClientHandle)
        {
            IOTHUB_CLIENT_RESULT result = IoTHubModuleClient_SetModuleMethodCallback(_moduleClientHandle, directMethodCallback, ctx);
            if (result != IOTHUB_CLIENT_OK)
            {
                throw DMException(DMSubsystem::IotHub, result, "IoTHubModuleClient_SetModuleMethodCallback failed!");
            }

            result = IoTHubModuleClient_SetModuleTwinCallback(_moduleClientHandle, twinCallback, ctx);
            if (result != IOTHUB_CLIENT_OK)
            {
                throw DMException(DMSubsystem::IotHub, result, "IoTHubModuleClient_SetModuleTwinCallback failed!");
            }
        }
    }

    void AzureRawHost::RegisterStaticHandler(
        std::shared_ptr<IRawHandler> handler)
    {
        TRACELINEP(LoggingLevel::Verbose, "Registering: ", handler->GetId().c_str());

        if (_rawHandlerMap.find(handler->GetId()) != _rawHandlerMap.end())
        {
            return;
        }

        handler->SetHandlerHost(_this);

        // Store in a handlerInfo struct...
        shared_ptr<RawHandlerInfo> rawHandlerInfo = make_shared<RawHandlerInfo>();
        rawHandlerInfo->_state = RawHandlerState::eInactive;
        rawHandlerInfo->_rawHandler = handler;

        _rawHandlerMap[handler->GetId()] = rawHandlerInfo;
    }

    void AzureRawHost::RegisterStaticHandlers()
    {
        RegisterStaticHandler(make_shared<ClearReportedCmdHandler>(
            [&](vector<string>& handlerIds) {
                GetHandlerIds(handlerIds);
            },
            [&](const Json::Value& reportedProperties) {
                ReportAll(reportedProperties);
            }));

        RegisterStaticHandler(make_shared<DeviceSchemasHandler>(&_rawHandlerMap));
    }

    void AzureRawHost::SetHardwareIds(
        std::shared_ptr<std::map<std::string, std::vector<std::string>>> handlersHardwareIds)
    {
        // Raw doesn't support hardware enumeration.
        _handlersHardwareIds = handlersHardwareIds;
    }

    void AzureRawHost::RegisterDynamicHandler(
        shared_ptr<IPlugin> plugin,
        const HandlerInfo& handlerInfo)
    {
        if (_rawHandlerMap.find(handlerInfo.id) != _rawHandlerMap.end())
        {
            return;
        }

        shared_ptr<IRawHandler> handler = plugin->CreateRawHandler(handlerInfo.id);

        // Set callbacks and register...
        handler->SetHandlerHost(_this);

        // Store in a handlerInfo struct...
        shared_ptr<RawHandlerInfo> rawHandlerInfo = make_shared<RawHandlerInfo>();
        rawHandlerInfo->_state = RawHandlerState::eInactive;
        rawHandlerInfo->_rawHandler = handler;
        rawHandlerInfo->_dependencies = handlerInfo.dependencies;

        _rawHandlerMap[handlerInfo.id] = rawHandlerInfo;
    }

    void AzureRawHost::HandlerInvokeSequence()
    {
        DependencySort depSort;
        for (auto& it : _rawHandlerMap)
        {
            depSort.SetDependenciesMap(it.first, it.second->_dependencies);
        }
        _rawHandlerSequence = depSort.SortDependency();
    }

    void AzureRawHost::UnregisterDynamicHandler(
        const string& handlerId)
    {
        TRACELINEP(LoggingLevel::Verbose, "Unregistering Id   : ", handlerId.c_str());
        TRACELINE(LoggingLevel::Verbose, "Unregistering Type : raw");

        auto it = _rawHandlerMap.find(handlerId);
        if (it != _rawHandlerMap.end())
        {
            _rawHandlerMap.erase(handlerId);
        }
    }

    void AzureRawHost::Report(
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
        ReportAll(reportedProperties);
    }

    void AzureRawHost::GetHandlerIds(vector<string>& handlerIds)
    {
        handlerIds.clear();

        for (RawHandlerMapType::const_iterator handlerIt = _rawHandlerMap.cbegin(); handlerIt != _rawHandlerMap.cend(); ++handlerIt)
        {
            handlerIds.emplace_back(handlerIt->first);
        }
    }

    void AzureRawHost::ReportAll(const Json::Value& partialReportedPropertiesObject)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        ReportAll(partialReportedPropertiesObject.toStyledString());
    }

    void AzureRawHost::ReportAll(const string& partialReportedPropertiesString)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        TRACELINE(LoggingLevel::Verbose, partialReportedPropertiesString.c_str());

        if (_deviceClientHandle != NULL)
        {
            IOTHUB_CLIENT_RESULT result = IoTHubDeviceClient_SendReportedState(_deviceClientHandle, (const unsigned char*)partialReportedPropertiesString.c_str(), partialReportedPropertiesString.size(), OnReportedStateSent, this);
            if (result != IOTHUB_CLIENT_OK)
            {
                throw DMException(DMSubsystem::IotHub, result, "IoTHubDeviceClient_SendReportedState failed!");
            }
        }
        else if (_moduleClientHandle != NULL)
        {
            IOTHUB_CLIENT_RESULT result = IoTHubModuleClient_SendReportedState(_moduleClientHandle, (const unsigned char*)partialReportedPropertiesString.c_str(), partialReportedPropertiesString.size(), OnReportedStateSent, this);
            if (result != IOTHUB_CLIENT_OK)
            {
                throw DMException(DMSubsystem::IotHub, result, "IoTHubModuleClient_SendReportedState failed!");
            }
        }
    }

    void AzureRawHost::OnReportedStateSent(int status_code, void* userContextCallback)
    {
        AzureRawHost* This = static_cast<AzureRawHost*>(userContextCallback);
        This->OnReportedStateSent(status_code);
    }

    void AzureRawHost::SendEvent(
        const std::string& handlerId,
        const std::string& eventName,
        const Json::Value& messageData)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;

        Json::Value message(messageData);
        std::string messageString = message.toStyledString();
        IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(messageString.c_str());
        if (messageHandle != NULL)
        {
            IoTHubMessage_SetContentTypeSystemProperty(messageHandle, "application%2fjson");
            IoTHubMessage_SetContentEncodingSystemProperty(messageHandle, "utf-8");

            IoTHubMessage_SetMessageId(messageHandle, eventName.c_str());

            MAP_HANDLE propMap = IoTHubMessage_Properties(messageHandle);
            Map_AddOrUpdate(propMap, "HandlerId", handlerId.c_str());
            Map_AddOrUpdate(propMap, "$$ContentType", "JSON");

            if (_deviceClientHandle != NULL)
            {
                result = IoTHubDeviceClient_SendEventAsync(_deviceClientHandle, messageHandle, OnSendEventSent, this);
            }
            if (_moduleClientHandle != NULL)
            {
                result = IoTHubModuleClient_SendEventAsync(_moduleClientHandle, messageHandle, OnSendEventSent, this);
            }

            IoTHubMessage_Destroy(messageHandle);
        }
        else
        {
            // Cannot throw exceptions here since we don't want to break the SDK loop.
            TRACELINE(LoggingLevel::Error, "Failed to create message.");
        }

        if (result != IOTHUB_CLIENT_OK)
        {
            // Cannot throw exceptions here since we don't want to break the SDK loop.
            TRACELINEP(LoggingLevel::Error, "IoTHubDeviceClient_SendEventAsync failed.", result);
        }
    }

    void AzureRawHost::OnDeviceTwinReceived(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t size, void* userContextCallback)
    {
        AzureRawHost* This = static_cast<AzureRawHost*>(userContextCallback);
        This->OnDeviceTwinReceived(updateState, payload, size);
    }

    int AzureRawHost::OnDirectMethodInvoked(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback)
    {
        AzureRawHost* This = static_cast<AzureRawHost*>(userContextCallback);
        return This->OnDirectMethodInvoked(methodName, payload, size, response, responseSize);
    }

    void AzureRawHost::OnSendEventSent(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
    {
        AzureRawHost* This = static_cast<AzureRawHost*>(userContextCallback);
        This->OnSendEventSent(result);
    }

    void AzureRawHost::OnSendEventSent(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        TRACELINEP(LoggingLevel::Verbose, "OnSendEventSent() - result: ", result);
    }

    void AzureRawHost::OnReportedStateSent(int statusCode)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        TRACELINEP(LoggingLevel::Verbose, "OnReportedStateSent() - statusCode: ", statusCode);
    }

    void AzureRawHost::MarkStatus(const Json::Value& desiredProperties, DeploymentStatus status)
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
                _reportedSectionsSummary->SetSectionStatus(handlerId, status);

                handlerIt->second->_rawHandler->SetDeploymentStatus(status);
                reportedProperties[handlerId] = handlerIt->second->_rawHandler->GetDeploymentStatusJson();
            }
            catch (const exception& ex)
            {
                TRACELINEP(LoggingLevel::Verbose, "Error: ", ex.what());

                // noexcept
                _reportedSectionsSummary->SetSectionStatus(handlerId, DeploymentStatus::eUnknown);

                // indicate something went wrong...
                reportedProperties[handlerId] = MetaData::DeploymentStatusToJsonObject(DeploymentStatus::eUnknown);
            }
        }

        reportedProperties[_reportedSectionsSummary->GetId()] = _reportedSectionsSummary->ToJsonObject();
        ReportAll(reportedProperties);
    }

    InvokeHandlerResult AzureRawHost::InvokeHandler(
        const std::string& handlerId,
        const Json::Value& parametersJson)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINEP(LoggingLevel::Verbose, "Looking for handler for: ", handlerId.c_str());

        InvokeHandlerResult invokeHandlerResult;

        try
        {
            RawHandlerMapType::const_iterator handlerIt = _rawHandlerMap.find(handlerId);
            if (handlerIt == _rawHandlerMap.cend())
            {
                TRACELINE(LoggingLevel::Verbose, "Handler not found.");
                return invokeHandlerResult;
            }
            invokeHandlerResult.found = true;

            TRACELINEP(LoggingLevel::Verbose, "Found handler for: ", handlerId.c_str());

            if (handlerIt->second->_state == RawHandlerState::eInactive)
            {
                TRACELINE(LoggingLevel::Verbose, "Handler is inactive.");
                return invokeHandlerResult;
            }

            invokeHandlerResult.active = true;

            invokeHandlerResult.invokeResult = handlerIt->second->_rawHandler->Invoke(parametersJson);
        }
        catch (const DMException& ex)
        {
            LogDMException(ex, "An error occured while invoking handler: ", handlerId.c_str());
        }
        catch (const exception& ex)
        {
            LogStdException(ex, "An error occured while invoking: ", handlerId.c_str());
        }

        return invokeHandlerResult;
    }

    void AzureRawHost::OnDeviceTwinReceived(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* buffer, size_t size)
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
        MarkStatus(desiredProperties, DeploymentStatus::ePending);

        // Go through the "desired" children and apply the desired changes...
        vector<string> keys = desiredProperties.getMemberNames();
        if(_rawHandlerSequence.size() == 0 && _rawHandlerMap.size() != 0)
        {
            MarkStatus(desiredProperties, DeploymentStatus::eFailed);
            TRACELINE(LoggingLevel::Verbose, "Failed to sort the handlers based on their dependency.");
        }
        else
        {

            for (const string& handlerKey : _rawHandlerSequence)
            {
                if (find(keys.begin(), keys.end(), handlerKey) != keys.end())
                {
                    TRACELINEP(LoggingLevel::Verbose, "Processing: ", handlerKey.c_str());
                    InvokeHandlerResult result = InvokeHandler(handlerKey, desiredProperties[handlerKey]);
                    if (result.found && result.active)
                    {
                        assert(!result.invokeResult.present);    // present == true is only for direct methods.
                    }
                }
            }
        }

        TRACELINE(LoggingLevel::Verbose, "---- Desired Properties End << ------------------------------------------------------------------------");
    }

    int AzureRawHost::OnDirectMethodInvoked(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINE(LoggingLevel::Verbose, "---- Direct Method Start >> ---------------------------------------------------------------------------");

        int returnCode = JsonDirectMethodSuccessCode;
        string parameters((const char*)payload, size);

        TRACELINEP(LoggingLevel::Verbose, "Device Method Name:", methodName);
        TRACELINEP(LoggingLevel::Verbose, "Parameters        :", parameters.c_str());

        Json::Value parametersJson = JsonUtils::JsonObjectFromString(parameters);

        InvokeHandlerResult result = InvokeHandler(methodName, parametersJson);
        if (!result.found)
        {
            result.invokeResult.payload = "{ \"message\": \"Handler not found.\" }";
            result.invokeResult.code = JsonDirectMethodFailureCode;
        }
        else if (!result.active)
        {
            result.invokeResult.payload = "{ \"message\": \"Handler not active.\" }";
            result.invokeResult.code = JsonDirectMethodFailureCode;
        }
        else
        {
            assert(result.invokeResult.present);
        }

        TRACELINEP(LoggingLevel::Verbose, "Device Method Returned->payload: ", result.invokeResult.payload.c_str());
        TRACELINEP(LoggingLevel::Verbose, "Device Method Returned->code: ", result.invokeResult.code);

        // Prepare the return payload...
        *responseSize = result.invokeResult.payload.size();
        if ((*response = (unsigned char*)malloc(*responseSize)) == NULL)
        {
            returnCode = -1;
            result.invokeResult.payload = "Out of memory";  // This doesn't really go anywhere.
        }
        else
        {
            memcpy(*response, result.invokeResult.payload.c_str(), *responseSize);
            returnCode = result.invokeResult.code;
        }

        TRACELINE(LoggingLevel::Verbose, "---- Direct Method End << -----------------------------------------------------------------------------");

        return returnCode;
    }

    void AzureRawHost::SignalStart()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        for (auto& it : _rawHandlerMap)
        {
            TRACELINEP(LoggingLevel::Verbose, "Starting: ", it.first.c_str());

            const string& handlerId = it.second->_rawHandler->GetId();

            try
            {
                Json::Value handlerConfig = BuildHandlerConfig(handlerId);

                bool active = false;
                it.second->_rawHandler->Start(handlerConfig, active);
                it.second->_state = active ? RawHandlerState::eActive : RawHandlerState::eInactive;
            }
            catch (const DMException& ex)
            {
                LogDMException(ex, "An error occured while starting: ", handlerId.c_str());
            }
            catch (const exception& ex)
            {
                LogStdException(ex, "An error occured while starting: ", handlerId.c_str());
            }
        }
    }

    void AzureRawHost::SignalStop()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        for (auto& it : _rawHandlerMap)
        {
            TRACELINEP(LoggingLevel::Verbose, "Stopping: ", it.first.c_str());
            it.second->_rawHandler->Stop();
        }
    }

    void AzureRawHost::SignalConnectionStatusChanged(
        ConnectionStatus status)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        for (auto& it : _rawHandlerMap)
        {
            TRACELINEP(LoggingLevel::Verbose, "Connection Status Changed: ", it.first.c_str());

            if (it.second->_state != RawHandlerState::eActive)
            {
                TRACELINE(LoggingLevel::Verbose, "Not active. Skipping...");
                continue;
            }

            const string& handleId = it.second->_rawHandler->GetId();

            try
            {
                it.second->_rawHandler->OnConnectionStatusChanged(status);
            }
            catch (const DMException& ex)
            {
                LogDMException(ex, "An error occured while starting: ", handleId.c_str());
            }
            catch (const exception& ex)
            {
                LogStdException(ex, "An error occured while starting: ", handleId.c_str());
            }
        }
    }

    void AzureRawHost::Destroy()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        if (_deviceClientHandle != NULL)
        {
            IoTHubDeviceClient_Destroy(_deviceClientHandle);
            _deviceClientHandle = NULL;
        }
        if (_moduleClientHandle != NULL)
        {
            IoTHubModuleClient_Destroy(_moduleClientHandle);
            _moduleClientHandle = NULL;
        }
    }

}}}}
