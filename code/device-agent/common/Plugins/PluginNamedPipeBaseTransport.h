// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <map>
#include "PluginInterfaces.h"
#include "MessageQueue.h"

// #define DEBUG_TRANSPORT

#ifdef DEBUG_TRANSPORT
#define TRANSPORT_TRACELINE(loggingLevel__, message__) \
   {\
        stringstream ss;\
        ss << "[Transport Layer] [";\
        ss << "0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << (void*)this;\
        ss << "] [" << _debugIdentity << "] ";\
        ss << message__; \
        TRACELINE(loggingLevel__, ss.str().c_str());\
   }\

#define TRANSPORT_TRACELINEP(loggingLevel__, message__, param__) \
   {\
        stringstream ss;\
        ss << "[Transport Layer] [";\
        ss << "0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << (void*)this;\
        ss << "] [" << _debugIdentity << "] ";\
        ss << message__; \
        TRACELINEP(loggingLevel__, ss.str().c_str(), param__);\
   }\

#else
#define TRANSPORT_TRACELINE
#define TRANSPORT_TRACELINEP
#endif

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class PluginNamedPipeBaseTransport : public IPluginTransport
    {
    public:
        PluginNamedPipeBaseTransport(
            const std::wstring& clientPipeName,
            const std::wstring& pluginPipeName);

        ~PluginNamedPipeBaseTransport();

        std::shared_ptr<Message> SendAndGetResponse(
            std::shared_ptr<Message> message);

        HANDLE GetShutdownNotificationEvent();

    protected:

        virtual void ProcessRequestMessage(
            std::shared_ptr<Message> message) = 0;

        virtual std::string GetDebugIdentity();

        void InitializeDequeEvents();

        // Monitoring threads
        void RestartMonitorThreads();

        // Monitors incoming messages...
        static void MessageHandlerThreadProc(
            void* context);
        void RunMessageMonitor();

        // Dequeues messages of type 'request' and processes them...
        static void RequestProcessorThreadProc(
            void* context);
        void RunRequestMonitor();

        // Read/Write Methods
        void WriteMessage(
            std::shared_ptr<Message> message);

        std::shared_ptr<Message> ReadMessage(
            HANDLE hPipe);

        void MessageToPackets(
            std::shared_ptr<Message> message,
            std::vector<Packet>& packets);

        std::shared_ptr<Message> PacketsToMessage(
            const std::vector<Packet>& packets);

        // Clean-up Methods
        virtual void StopMonitorThreads();
        void ClosePipes();
        void CloseDequeEvents();
        void CloseTransport();

        // Data members
        std::recursive_mutex _mutex;

        std::string _debugIdentity;

        std::wstring _clientPipeName;
        std::wstring _pluginPipeName;

        HANDLE _hWritePipe;
        HANDLE _hReadPipe;

        DMUtils::JoiningThread _pluginHostMonitoringThread;
        DMUtils::JoiningThread _messageHandlerThread;
        DMUtils::JoiningThread _requestProcessorThread;

        HANDLE _hResponseNotification;
        HANDLE _hRequestNotification;
        HANDLE _hShutdownNotification;

        MessageQueue _responseQueue;
        MessageQueue _requestQueue;
    };

}}}}
