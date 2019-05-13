// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "PluginNamedPipeBaseTransport.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class PluginNamedPipeAgentTransport : public PluginNamedPipeBaseTransport
    {
    public:
        PluginNamedPipeAgentTransport(
            const std::wstring& pluginPath,
            long keepAliveTime,
            const std::wstring& clientPipeName,
            const std::wstring& pluginPipeName,
            IPluginStateMonitor* pluginStateMonitor);

        ~PluginNamedPipeAgentTransport();

    private:

        // IPluginTransport

        void Initialize();

        std::shared_ptr<Message> SendAndGetResponse(
            std::shared_ptr<Message> message);

        void SetClientInterface(
            PluginReverseInvokePtr reverseInvokePtr, PluginReverseDeleteBufferPtr reverseDeletePtr);

        void SetPluginInterface(
            PluginCreatePtr, PluginInvokePtr, PluginDeleteBufferPtr);

        // Monitoring threads
        static void PluginHostMonitoringThreadProc(
            void* context);
    
        void RunPluginHostMonitor();

        // PluginNamedPipeBaseTransport virtuals

        void StopMonitorThreads();

        void ProcessRequestMessage(
            std::shared_ptr<Message> requestMessage);

        // Helpers
        void LaunchPluginHost();

        void SendShutdownToPluginHost();

        // Data members

        long _keepAliveTime;
        std::wstring _pluginPath;

        bool _pluginHostInitialized;
        PROCESS_INFORMATION _processInfo;

        IPluginStateMonitor* _pluginStateMonitor;

        PluginReverseInvokePtr _pluginReverseInvoke;
        PluginReverseDeleteBufferPtr _pluginReverseDeleteBuffer;
    };

}}}}
