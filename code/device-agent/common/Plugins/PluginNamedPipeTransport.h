// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <map>
#include "PluginInterfaces.h"
#include "MessageQueue.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

class PluginNamedPipeTransport : public IPluginTransport
{
public:
    PluginNamedPipeTransport(
        bool client,
        const std::wstring& pluginPath,
        long keepAliveTime,
        const std::wstring& clientPipeName,
        const std::wstring& pluginPipeName,
        IPluginStateMonitor* pluginStateMonitor);

    void Initialize();

    std::shared_ptr<Message> SendAndGetResponse(std::shared_ptr<Message>);
    void SetClientInterface(PluginReverseInvokePtr, PluginReverseDeleteBufferPtr);
    void SetPluginInterface(PluginCreatePtr, PluginInvokePtr, PluginDeleteBufferPtr);
    void CloseTransport();
    HANDLE GetShutdownNotificationEvent();

private:
    void LaunchPluginHost();
    void ClientInitialization();
    void PluginInitialization();
    void ClientUninitializePlugin();
    std::shared_ptr<Message> SendMessageWorker(std::shared_ptr<Message>);
    void ProcessRequest();
    void MessageHandlerWorker();
    void PluginHostMonitor();
    void ProcessRequestMessage(std::shared_ptr<Message>);

    // Worker threads
    static void PluginHostMonitoringThreadProc(void* context)
    {
        PluginNamedPipeTransport* transport = static_cast<PluginNamedPipeTransport*>(context);
        transport->PluginHostMonitor();
    }

    static void MessageHandlerThreadProc(void* context)
    {
        PluginNamedPipeTransport* transport = static_cast<PluginNamedPipeTransport*>(context);
        transport->MessageHandlerWorker();
    }

    static void RequestProcessorThreadProc(void* context)
    {
        PluginNamedPipeTransport* transport = static_cast<PluginNamedPipeTransport*>(context);
        transport->ProcessRequest();
    }

private:
    std::recursive_mutex _mutex;
    bool _client;
    long _keepAliveTime;
    bool _pluginHostInitialized;
    bool _pluginCreated;
    std::wstring _pluginPath;
    std::wstring _clientPipeName;
    std::wstring _pluginPipeName;

    IPluginStateMonitor* _pluginStateMonitor;

    PROCESS_INFORMATION _processInfo;
    HANDLE _hClientPipe;
    HANDLE _hPluginPipe;

    DMUtils::JoiningThread _pluginHostMonitoringThread;
    DMUtils::JoiningThread _messageHandlerThread;
    DMUtils::JoiningThread _requestProcessorThread;

    HANDLE _hResponseNotification;
    HANDLE _hRequestNotification;
    HANDLE _hShutdownNotification;

    MessageQueue _response;
    MessageQueue _request;

    // Function pointers for message handling
    PluginCreatePtr _pluginCreate;
    PluginInvokePtr _pluginInvoke;
    PluginDeleteBufferPtr _pluginDeleteBuffer;
    PluginReverseInvokePtr _pluginReverseInvoke;
    PluginReverseDeleteBufferPtr _pluginReverseDeleteBuffer;
};
}}}}
