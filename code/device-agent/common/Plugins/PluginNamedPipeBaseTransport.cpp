// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <iomanip>
#include "PluginNamedPipeBaseTransport.h"
#include "PluginConstants.h"
#include "PluginJsonConstants.h"
#include "CrossBinaryRequest.h"

#define ERROR_PIPE_HAS_ENDED 109

using namespace std;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

PluginNamedPipeBaseTransport::PluginNamedPipeBaseTransport(
    const std::wstring& clientPipeName,
    const std::wstring& pluginPipeName) :
    _debugIdentity(""),
    _hWritePipe(INVALID_HANDLE_VALUE),
    _hReadPipe(INVALID_HANDLE_VALUE),
    _hResponseNotification(INVALID_HANDLE_VALUE),
    _hRequestNotification(INVALID_HANDLE_VALUE),
    _hShutdownNotification(INVALID_HANDLE_VALUE),
    _clientPipeName(clientPipeName),
    _pluginPipeName(pluginPipeName)
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    if (clientPipeName.empty())
    {
        _clientPipeName = Guid::NewGuid();
    }

    if (pluginPipeName.empty())
    {
        _pluginPipeName = Guid::NewGuid();
    }
}

PluginNamedPipeBaseTransport::~PluginNamedPipeBaseTransport()
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    CloseTransport();
}

void PluginNamedPipeBaseTransport::InitializeDequeEvents()
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    if (_hResponseNotification != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hResponseNotification);
    }
    _hResponseNotification = CreateEvent(nullptr, FALSE /*auto reset*/, FALSE /*not set*/, nullptr);

    if (_hRequestNotification != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hRequestNotification);
    }
    _hRequestNotification = CreateEvent(nullptr, FALSE /*auto reset*/, FALSE /*not set*/, nullptr);

    if (_hShutdownNotification != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hShutdownNotification);
    }
    _hShutdownNotification = CreateEvent(nullptr, TRUE /*manual reset*/, FALSE /*not set*/, nullptr);
}

void PluginNamedPipeBaseTransport::RestartMonitorThreads()
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Create a message handling thread
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Spawning MessageHandlerThreadProc().");
    _messageHandlerThread.Join();
    _messageHandlerThread = std::thread(PluginNamedPipeBaseTransport::MessageHandlerThreadProc, this);

    TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Spawning RequestProcessorThreadProc().");
    _requestProcessorThread.Join();
    _requestProcessorThread = std::thread(PluginNamedPipeBaseTransport::RequestProcessorThreadProc, this);
}

string PluginNamedPipeBaseTransport::GetDebugIdentity()
{
    return _debugIdentity;
}

HANDLE PluginNamedPipeBaseTransport::GetShutdownNotificationEvent()
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
    return _hShutdownNotification;
}

std::shared_ptr<Message> PluginNamedPipeBaseTransport::SendAndGetResponse(std::shared_ptr<Message> message)
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    WriteMessage(message);

    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__ " - Waiting for a response...");
    // Wait for response
    WaitForSingleObject(_hResponseNotification, INFINITE);

    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__ " - Received response...");
    return _responseQueue.Pop();
}

void PluginNamedPipeBaseTransport::MessageToPackets(
    shared_ptr<Message> message,
    vector<Packet>& packets)
{
    // Create a single buffer to hold the data we neet to transfer.
    size_t dataToWriteSize = Message::HeaderSize() + message->PayloadSize();
    vector<char> dataToWrite(dataToWriteSize);
    memcpy(dataToWrite.data(), message->Header(), Message::HeaderSize());
    memcpy(dataToWrite.data() + Message::HeaderSize(), message->Payload(), message->PayloadSize());

    // Calcualte how many packets we'll need...
    size_t maxPacketPayloadSize = Packet::MaxPayloadSize();
    size_t packetCount = (dataToWriteSize / maxPacketPayloadSize) + 1;

    // Create the packets...
    for (size_t i = 0; i < packetCount; ++i)
    {
        Packet packet;
        packet.index = i;
        packet.count = packetCount;
        packet.payloadSize = dataToWriteSize > maxPacketPayloadSize ? maxPacketPayloadSize : dataToWriteSize;
        memcpy(packet.payload, dataToWrite.data() + i * maxPacketPayloadSize, packet.payloadSize);

        packets.push_back(packet);

        dataToWriteSize -= maxPacketPayloadSize;
    }
}

std::shared_ptr<Message> PluginNamedPipeBaseTransport::PacketsToMessage(
    const std::vector<Packet>& packets)
{
    // Get the total size of the payload
    size_t payloadSize = 0;
    for (size_t i = 0; i < packets.size(); ++i)
    {
        payloadSize += packets[i].payloadSize;
    }

    // Reconstruct the message payload
    vector<char> payload(payloadSize);
    for (size_t i = 0; i < packets.size(); ++i)
    {
        memcpy(payload.data() + i * Packet::MaxPayloadSize(), packets[i].payload, packets[i].payloadSize);
    }

    // Reconstruct the message
    std::shared_ptr<Message> message = std::make_shared<Message>();
    memcpy(message.get(), payload.data(), Message::HeaderSize());
    message->SetData(payload.data() + Message::HeaderSize(), payload.size() - Message::HeaderSize());
    return message;
}

void PluginNamedPipeBaseTransport::WriteMessage(
    std::shared_ptr<Message> message)
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    vector<Packet> packets;
    MessageToPackets(message, packets);

    for (size_t i = 0; i < packets.size(); ++i)
    {
        DWORD cbWritten = 0;
        DWORD cbToWrite = static_cast<DWORD>(Packet::HeaderSize() + packets[i].payloadSize);
        // Write the reply to the pipe.
        BOOL fSuccess = WriteFile(
            _hWritePipe,         // handle to pipe
            &packets[i],   // buffer to write from
            cbToWrite,     // number of bytes to write
            &cbWritten,    // number of bytes written
            nullptr);      // not overlapped I/O

        if (!fSuccess || cbToWrite != cbWritten)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "WriteFile to named pipe failed");
        }
    }
}

shared_ptr<Message> PluginNamedPipeBaseTransport::ReadMessage(HANDLE hPipe)
{
    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;

    vector<Packet> packets;
    do
    {
        Packet packet;

        BOOL fSuccess = ReadFile(
            hPipe,                 // handle to pipe
            &packet,               // buffer to receive data
            static_cast<DWORD>(Packet::HeaderSize()), // size of buffer
            &cbBytesRead,          // number of bytes read
            nullptr);              // not overlapped I/O

        DWORD error = GetLastError();

        if ((!fSuccess && error != ERROR_MORE_DATA) || cbBytesRead == 0)
        {
            string msg;
            msg += "[Transport Layer] ";
            if (error == ERROR_PIPE_HAS_ENDED)
            {
                msg += "Input pipe has ended.";
                TRANSPORT_TRACELINE(LoggingLevel::Verbose, msg.c_str() );
            }
            else
            {
                msg += "Failed to read packet header from the pipe. Error = ";
                TRANSPORT_TRACELINEP(LoggingLevel::Verbose, msg.c_str(), error);
            }
            return nullptr;
        }

        if (packet.payloadSize != 0)
        {
            fSuccess = ReadFile(
                hPipe,                 // handle to pipe
                packet.payload,        // buffer to receive data
                static_cast<DWORD>(packet.payloadSize),    // size of buffer
                &cbBytesRead,          // number of bytes read
                nullptr);              // not overlapped I/O

            DWORD error = GetLastError();

            if (!fSuccess || cbBytesRead == 0)
            {
                wstring msg;
                msg += L"[Transport Layer] ";
                if (error == ERROR_PIPE_HAS_ENDED)
                {
                    msg += L"Input pipe has ended.";
                    TRANSPORT_TRACELINE(LoggingLevel::Verbose, msg.c_str());
                }
                else
                {
                    msg += L"Failed to read packet header from the pipe. Error = ";
                    TRANSPORT_TRACELINEP(LoggingLevel::Verbose, msg.c_str(), error);
                }
                return nullptr;
            }
        }

        packets.push_back(packet);

        // Is this the last packet of this message?
        if (packet.index == packet.count - 1)
        {
            break;
        }

    } while (true);

    return PacketsToMessage(packets);
}

void PluginNamedPipeBaseTransport::RunMessageMonitor()
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    while (1)
    {
        try
        {
            std::shared_ptr<Message> incomingMessage = ReadMessage(_hReadPipe);
            if (incomingMessage == nullptr)
            {
                TRANSPORT_TRACELINE(LoggingLevel::Verbose, "ReadMessage() returned an empty message - RunMessageMonitor is returning...");
                return;
            }

            if (incomingMessage->messageType == Request)
            {
                TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Recieved a Request Message");
                _requestQueue.Push(incomingMessage);
                SetEvent(_hRequestNotification);
            }
            else if (incomingMessage->messageType == Response)
            {
                TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Recieved a Response Message");
                _responseQueue.Push(incomingMessage);
                SetEvent(_hResponseNotification);
            }
        }
        catch (const DMException& ex)
        {
            LogDMException(ex, "An error occured while reading incoming messages.", "");
        }
        catch (const exception& ex)
        {
            LogStdException(ex, "An error occured while reading incoming messages.", "");
        }
    }

    TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Exiting incoming message monitor thread.");
}

void PluginNamedPipeBaseTransport::MessageHandlerThreadProc(void* context)
{
    PluginNamedPipeBaseTransport* transport = static_cast<PluginNamedPipeBaseTransport*>(context);
    transport->RunMessageMonitor();
}

void PluginNamedPipeBaseTransport::RunRequestMonitor()
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    while (1)
    {
        try
        {
            HANDLE hEvents[2];
            hEvents[0] = _hRequestNotification;
            hEvents[1] = _hShutdownNotification;
            switch(WaitForMultipleObjects(2, hEvents, FALSE, INFINITE))
            {
                case WAIT_OBJECT_0:
                {
                    TRANSPORT_TRACELINE(LoggingLevel::Verbose, L"Received new request notification.");
                    std::shared_ptr<Message> requestMessage = _requestQueue.Pop();
                    if (requestMessage)
                    {
                        ProcessRequestMessage(requestMessage);
                    }
                    break;
                }
                case WAIT_OBJECT_0 + 1:
                default:
                {
                    TRANSPORT_TRACELINE(LoggingLevel::Verbose, L"Request processing thread received shutdown notification.");
                    return;
                }
            }
        }
        catch (const DMException& ex)
        {
            LogDMException(ex, "An error occured while processing incoming request.", "");
        }
        catch (const exception& ex)
        {
            LogStdException(ex, "An error occured while processing incoming request.", "");
        }
    }

    TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__ "Exiting...");
}

void PluginNamedPipeBaseTransport::RequestProcessorThreadProc(void* context)
{
    PluginNamedPipeBaseTransport* transport = static_cast<PluginNamedPipeBaseTransport*>(context);
    transport->RunRequestMonitor();
}

void PluginNamedPipeBaseTransport::StopMonitorThreads()
{
    // Set the shutdown event
    if (_hShutdownNotification != INVALID_HANDLE_VALUE)
    {
        SetEvent(_hShutdownNotification);
    }

    // Cannot join self...
    if (this_thread::get_id() != _messageHandlerThread.GetId())
    {
        _messageHandlerThread.Join();
    }

    // Cannot join self...
    if (this_thread::get_id() != _requestProcessorThread.GetId())
    {
        _requestProcessorThread.Join();
    }
}

void PluginNamedPipeBaseTransport::CloseDequeEvents()
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
    if (_hResponseNotification != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hResponseNotification);
        _hResponseNotification = INVALID_HANDLE_VALUE;
    }

    if (_hRequestNotification != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hRequestNotification);
        _hRequestNotification = INVALID_HANDLE_VALUE;
    }
}

void PluginNamedPipeBaseTransport::ClosePipes()
{
    // Close the pipes
    if (_hReadPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hReadPipe);
        _hReadPipe = INVALID_HANDLE_VALUE;
    }

    if (_hWritePipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hWritePipe);
        _hWritePipe = INVALID_HANDLE_VALUE;
    }
}

void PluginNamedPipeBaseTransport::CloseTransport()
{
    TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    lock_guard<recursive_mutex> guard(_mutex);

    // Exit all monitoring threads
    StopMonitorThreads();

    // Sync events are no longer needed because threads have existed
    CloseDequeEvents();

    // Close the pipes
    ClosePipes();

    // Free the remaining shutdown event
    if (_hShutdownNotification != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hShutdownNotification);
        _hShutdownNotification = INVALID_HANDLE_VALUE;
    }
}

}}}}
