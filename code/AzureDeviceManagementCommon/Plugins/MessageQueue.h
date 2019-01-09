// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <assert.h>
#include <mutex>
#include <queue>
#include <Windows.h>
#include "..\Plugins\PluginInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {
    class MessageQueue
    {
    public:
        MessageQueue() {}
        void Push(std::shared_ptr<Message>);
        std::shared_ptr<Message> Pop();
        unsigned int Size();
        void Clear();

    private:
        std::queue<std::shared_ptr<Message>> _messageQueue;
        std::mutex _mutex;
    };
 }}}}
