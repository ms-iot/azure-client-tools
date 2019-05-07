// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <vector>
#include <Windows.h>
#include "../DMInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    enum MessageType
    {
        Undefined,
        Request,
        Response
    };

    enum CallType
    {
        None,
        PluginCreateCall,
        PluginInvokeCall,
        PluginShutdownCall,
        ReverseInvokeCall
    };

    class Packet
    {
    public:
        Packet();

        // Methods
        void* Header();
        static size_t HeaderSize();

        static size_t MaxPayloadSize();

        // Data
        size_t index;
        size_t count;
        size_t payloadSize;
        char payload[IPC_BUFFER_SIZE - sizeof(size_t) * 3];
    };

    class Message
    {
    public:
        Message();

        // Methods
        void* Header();
        static size_t HeaderSize();

        const char* Payload() const;
        size_t PayloadSize() const;

        void SetData(const char* newData, size_t length);

        MessageType messageType;
        CallType callType;
        DWORD errorCode;

    private:
        std::vector<char> data;
    };

}}}}
