// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Message.h"

using namespace std;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    Packet::Packet() :
        index(0),
        count(0),
        payloadSize(0)
    {}

    void* Packet::Header()
    {
        return this;
    }

    size_t Packet::HeaderSize()
    {
        return sizeof(size_t) * 3;
    }

    size_t Packet::MaxPayloadSize()
    {
        return sizeof(payload);
    }

    Message::Message() :
        messageType(Undefined),
        callType(None),
        errorCode(0)
    {
    }

    void* Message::Header()
    {
        return this;
    }

    size_t Message::HeaderSize()
    {
        return sizeof(messageType) + sizeof(callType) + sizeof(errorCode);
    }

    const char* Message::Payload() const
    {
        return data.data();
    }

    size_t Message::PayloadSize() const
    {
        return data.size();
    }

    void Message::SetData(const char* newData, size_t length)
    {
        data.resize(length);
        memcpy(data.data(), newData, length);
    }

}}}}
