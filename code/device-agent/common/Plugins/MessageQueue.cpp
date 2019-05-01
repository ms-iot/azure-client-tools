// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "MessageQueue.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

void MessageQueue::Push(std::shared_ptr<Message> message)
{
    std::lock_guard<std::mutex> guard(_mutex);
    _messageQueue.push(message);
}

std::shared_ptr<Message> MessageQueue::Pop()
{
    std::lock_guard<std::mutex> guard(_mutex);
    std::shared_ptr<Message> message;
    if (_messageQueue.size() > 0)
    {
        message = _messageQueue.front();
        _messageQueue.pop();
    }
    return message;
}

unsigned int MessageQueue::Size()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return (unsigned int)_messageQueue.size();
}

void MessageQueue::Clear()
{
    std::lock_guard<std::mutex> guard(_mutex);
    std::queue<std::shared_ptr<Message>> empty;
    std::swap(_messageQueue, empty);
}
}}}}
