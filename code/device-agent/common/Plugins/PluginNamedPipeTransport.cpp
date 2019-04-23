// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginNamedPipeTransport.h"
#include "PluginConstants.h"
#include "PluginJsonConstants.h"
#include "CrossBinaryRequest.h"

#define PLUGIN_SHUTDOWN_TIME 5000
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    PluginNamedPipeTransport::PluginNamedPipeTransport(
        bool client,
        const std::wstring& pluginPath,
        long keepAliveTime,
        const std::wstring& clientPipeName,
        const std::wstring& pluginPipeName,
        IPluginStateMonitor* pluginStateMonitor) :
        _client(client),
        _pluginPath(pluginPath),
        _keepAliveTime(keepAliveTime),
        _clientPipeName(clientPipeName),
        _pluginPipeName(pluginPipeName),
        _pluginHostInitialized(false),
        _pluginCreated(false),
        _pluginCreate(nullptr),
        _pluginInvoke(nullptr),
        _pluginDeleteBuffer(nullptr),
        _pluginReverseInvoke(nullptr),
        _pluginReverseDeleteBuffer(nullptr),
        _pluginStateMonitor(pluginStateMonitor)
{
    _hResponseNotification = CreateEvent(nullptr, FALSE /*auto reset*/, FALSE /*not set*/, nullptr);
    _hRequestNotification  = CreateEvent(nullptr, FALSE /*auto reset*/, FALSE /*not set*/, nullptr);
    _hShutdownNotification = CreateEvent(nullptr, FALSE /*auto reset*/, FALSE /*not set*/, nullptr);

    if (clientPipeName.empty())
    {
        _clientPipeName = Guid::NewGuid();
    }

    if (pluginPipeName.empty())
    {
        _pluginPipeName = Guid::NewGuid();
    }
}

void PluginNamedPipeTransport::Initialize()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] "  __FUNCTION__);

    // If this is a client call and plugin is already initialized return
    if (_client && _pluginHostInitialized)
    {
        TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Plugin already initialized.");
        return;
    }

    if (_client)
    {
        ClientInitialization();
    }
    else
    {
        PluginInitialization();
    }

    // Create a message handling thread
    _messageHandlerThread.Join();
    _messageHandlerThread = std::thread(PluginNamedPipeTransport::MessageHandlerThreadProc, this);

    _requestProcessorThread.Join();
    _requestProcessorThread = std::thread(PluginNamedPipeTransport::RequestProcessorThreadProc, this);

    // Notify listeners...
    if (_pluginStateMonitor)
    {
        _pluginStateMonitor->OnPluginLoaded();
    }
}

void PluginNamedPipeTransport::ClientInitialization()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    std::wstring clientPipeFullName = L"\\\\.\\pipe\\" + _clientPipeName;
    std::wstring pluginPipeFullName = L"\\\\.\\pipe\\" + _pluginPipeName;

    // Create NamedPipe
    _hClientPipe = CreateNamedPipe(
        clientPipeFullName.c_str(),         // pipe name
        PIPE_ACCESS_DUPLEX,                 // Inbound plugin->client
        PIPE_TYPE_MESSAGE |                 // message type pipe
        PIPE_READMODE_MESSAGE |             // message-read mode
        PIPE_WAIT,                          // blocking mode
        PIPE_UNLIMITED_INSTANCES,           // max. instances
        BUFFER_SIZE,                        // output buffer size
        BUFFER_SIZE,                        // input buffer size
        0,                                  // client time-out
        nullptr);                           // default security attribute

    if (_hClientPipe == INVALID_HANDLE_VALUE)
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Unable to open client named pipe");
    }

    _hPluginPipe = CreateNamedPipe(
        pluginPipeFullName.c_str(), // pipe name
        PIPE_ACCESS_DUPLEX,                 // Outbound client->plugin
        PIPE_TYPE_MESSAGE |                 // message type pipe
        PIPE_READMODE_MESSAGE |             // message-read mode
        PIPE_WAIT,                          // blocking mode
        PIPE_UNLIMITED_INSTANCES,           // max. instances
        BUFFER_SIZE,                        // output buffer size
        BUFFER_SIZE,                        // input buffer size
        0,                                  // client time-out
        nullptr);                           // default security attribute

    if (_hPluginPipe == INVALID_HANDLE_VALUE)
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to create plugin named pipe");
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(_hClientPipe, &dwMode, nullptr, nullptr))
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "SetNamedPipeHandleState failed for client pipe");
    }

    if (!SetNamedPipeHandleState(_hPluginPipe, &dwMode, nullptr, nullptr))
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "SetNamedPipeHandleState failed for plugin pipe");
    }

    // Launch the plugin host
    LaunchPluginHost();

    TRACE(LoggingLevel::Verbose, "Connecting to client pipe...");
    ConnectNamedPipe(_hClientPipe, nullptr);

    TRACE(LoggingLevel::Verbose, "Connecting to plugin pipe...");
    ConnectNamedPipe(_hPluginPipe, nullptr);

    // Reset the events
    ResetEvent(_hRequestNotification);
    ResetEvent(_hResponseNotification);
    ResetEvent(_hShutdownNotification);

    // Clear the queues
    _request.Clear();
    _response.Clear();

    _pluginHostInitialized = true;
}

void PluginNamedPipeTransport::PluginInitialization()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    std::wstring clientPipeFullName = L"\\\\.\\pipe\\" + _clientPipeName;
    std::wstring pluginPipeFullName = L"\\\\.\\pipe\\" + _pluginPipeName;

    // Open the named pipes
    _hClientPipe = CreateFile(
        clientPipeFullName.c_str(),   // pipe name
        GENERIC_WRITE,                // write access (plugin->client)
        0,                            // no sharing
        nullptr,                      // default security attributes
        OPEN_EXISTING,                // opens existing pipe
        0,                            // default attributes
        nullptr);                     // no template file

    if (_hClientPipe == INVALID_HANDLE_VALUE)
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to open client named pipe");
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(_hClientPipe, &dwMode, nullptr, nullptr))
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to set the pipe readmode for client pipe");
    }

    _hPluginPipe = CreateFile(
        pluginPipeFullName.c_str(),   // pipe name
        GENERIC_READ,                 // read access (client->plugin)
        0,                            // no sharing
        nullptr,                      // default security attributes
        OPEN_EXISTING,                // opens existing pipe
        0,                            // default attributes
        nullptr);                     // no template file

    if (_hPluginPipe == INVALID_HANDLE_VALUE)
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to open plugin named pipe");
    }
}

void PluginNamedPipeTransport::SetPluginInterface(
    PluginCreatePtr createPtr,
    PluginInvokePtr invokePtr,
    PluginDeleteBufferPtr deleteBufferPtr)
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
    if (_client)
    {
        throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_OPERATION, "Set plugin interface should only be called by the plugin");
    }

    _pluginCreate = createPtr;
    _pluginInvoke = invokePtr;
    _pluginDeleteBuffer = deleteBufferPtr;
}

void PluginNamedPipeTransport::SetClientInterface(PluginReverseInvokePtr reverseInvokePtr, PluginReverseDeleteBufferPtr reverseDeletePtr)
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
    if (!_client)
    {
        throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_OPERATION, "Set reverse invoke should only be called by client");
    }

    _pluginReverseInvoke = reverseInvokePtr;
    _pluginReverseDeleteBuffer = reverseDeletePtr;
}

HANDLE PluginNamedPipeTransport::GetShutdownNotificationEvent()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
    return _hShutdownNotification;
}

std::shared_ptr<Message>PluginNamedPipeTransport::SendAndGetResponse(std::shared_ptr<Message> message)
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
    std::lock_guard<std::recursive_mutex> guard(_mutex);
    HANDLE hPipe = INVALID_HANDLE_VALUE;
    if (_client)
    {
        // If plugin is not initialized, initialize it first
        if (!_pluginHostInitialized || _hPluginPipe == INVALID_HANDLE_VALUE)
        {
            Initialize();
        }
    }
    return SendMessageWorker(message);
}

std::shared_ptr<Message> PluginNamedPipeTransport::SendMessageWorker(std::shared_ptr<Message> message)
{
    HANDLE hPipe = _client ? _hPluginPipe : _hClientPipe;
    DWORD cbWritten = 0;
    DWORD cbMessageSize = sizeof(Message);
    // Write the reply to the pipe.
    BOOL fSuccess = WriteFile(
        hPipe,         // handle to pipe
        message.get(), // buffer to write from
        cbMessageSize, // number of bytes to write
        &cbWritten,    // number of bytes written
        nullptr);      // not overlapped I/O

    if (!fSuccess || cbMessageSize != cbWritten)
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "WriteFile to named pipe failed");
    }

    // Wait for response
    WaitForSingleObject(_hResponseNotification, INFINITE);
    std::shared_ptr<Message> response = _response.Pop();
    return response;
}

void PluginNamedPipeTransport::LaunchPluginHost()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    ZeroMemory(&_processInfo, sizeof(PROCESS_INFORMATION));

    STARTUPINFO startupInfo;
    ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
    startupInfo.cb = sizeof(STARTUPINFO);

    wchar_t fileName[MAX_PATH];
    GetModuleFileName(NULL, fileName, MAX_PATH);
    std::wstring moduleFileName = std::wstring(fileName);
    std::wstring processId = std::to_wstring(GetCurrentProcessId());
    std::wstring::size_type pos = moduleFileName.find_last_of(L"\\/") + 1;
    std::wstring cmdLine = moduleFileName.substr(0, pos) + L"AzureDeviceManagementPluginHost.exe" +
        L" -pluginPath " + _pluginPath +
        L" -parentPid " + processId +
        L" -clientPipeName " + _clientPipeName +
        L" -pluginPipeName " + _pluginPipeName;
#if DEBUG_DEVICE_AGENT_ROUTING
    cmdLine += L" -logsPath <replace with logs path>"
#endif

    if (!CreateProcess(
        nullptr,
        const_cast<wchar_t*>(cmdLine.c_str()), // command line
        nullptr,                               // process security attributes
        nullptr,                               // primary thread security attributes
        TRUE,                                  // handles are inherited
        0,                                     // creation flags
        nullptr,                               // use parent's environment
        nullptr,                               // use parent's current directory
        &startupInfo,                          // STARTUPINFO pointer
        &_processInfo)) // receives PROCESS_INFORMATION
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to launch the plugin host");
    }

    CloseHandle(_processInfo.hThread);

    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Pluginhost has been launched.");

    // Create a process monitoring thread
    _pluginHostMonitoringThread.Join();

    _pluginHostMonitoringThread = std::thread(PluginNamedPipeTransport::PluginHostMonitoringThreadProc, this);

    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] Monitoring thread created.");
}

void PluginNamedPipeTransport::PluginHostMonitor()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    if (_processInfo.hProcess != nullptr)
    {
        HANDLE events[2];
        events[0] = _processInfo.hProcess;
        events[1] = _hShutdownNotification;
        DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, _keepAliveTime);
        switch (waitResult)
        {
            case WAIT_OBJECT_0:
            {
                // If the process exited, uninitialize the plugin
                TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Plugin host exited.");
                CloseHandle(_processInfo.hProcess);
                ZeroMemory(&_processInfo, sizeof(PROCESS_INFORMATION));
                ClientUninitializePlugin();
                return;
            }

            case WAIT_OBJECT_0 + 1:
            case WAIT_TIMEOUT:
            {
                std::lock_guard<std::recursive_mutex> guard(_mutex);

                // Timeout reached or received shutdown notification send a shutdown call and wait for process to exit
                std::shared_ptr<Message> shutdownMessage = std::make_shared<Message>();
                shutdownMessage->messageType = Request;
                shutdownMessage->callType = PluginShutdownCall;
                shutdownMessage->errorCode = DM_ERROR_SUCCESS;
                DWORD cbWritten = 0;
                DWORD cbMessageSize = sizeof(Message);
                BOOL fSuccess = WriteFile(
                    _hPluginPipe,          // handle to pipe
                    shutdownMessage.get(), // buffer to write from
                    cbMessageSize,         // number of bytes to write
                    &cbWritten,            // number of bytes written
                    nullptr);              // not overlapped I/O

                if (!fSuccess || cbMessageSize != cbWritten)
                {
                    TRACELINE(LoggingLevel::Error, "Failed to send shutdown message to the plugin");
                    TerminateProcess(_processInfo.hProcess, -1);
                }

                // Wait for 5 seconds for process to exit
                DWORD exitCode = -1;
                if (WAIT_OBJECT_0 == WaitForSingleObject(_processInfo.hProcess, PLUGIN_SHUTDOWN_TIME))
                {
                    exitCode = 0;
                }

                TerminateProcess(_processInfo.hProcess, exitCode);

                ClientUninitializePlugin();
                return;;
            }
        default:
            {
                TRACELINE(LoggingLevel::Error, "[Client] Wait for process handle failed. Exiting Monitoring thread");
                return;
            }
        }
    }
}

// Called by client to terminate connection to current instance of plugin host
void PluginNamedPipeTransport::ClientUninitializePlugin()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
    _pluginHostInitialized = false;

    SetEvent(_hShutdownNotification);

    // Close the pipes
    CloseHandle(_hClientPipe);
    _hClientPipe = INVALID_HANDLE_VALUE;
    CloseHandle(_hPluginPipe);
    _hPluginPipe = INVALID_HANDLE_VALUE;
}

void PluginNamedPipeTransport::MessageHandlerWorker()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    while (1)
    {
        if (_client && !_pluginHostInitialized)
        {
            TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Plugin is not initialized, exiting MessageHandlerWorker");
            return;
        }

        DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
        std::shared_ptr<Message> incomingMessage = std::make_shared<Message>();
        // If client start reading messages on client pipe else if plugin read messages from pluginpipe
        HANDLE hPipe = _client ? _hClientPipe : _hPluginPipe;

        if (hPipe != INVALID_HANDLE_VALUE)
        {
            BOOL fSuccess = ReadFile(
                hPipe,                 // handle to pipe
                incomingMessage.get(), // buffer to receive data
                sizeof(Message),       // size of buffer
                &cbBytesRead,          // number of bytes read
                nullptr);              // not overlapped I/O

            if (!fSuccess || cbBytesRead == 0)
            {
                TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Failed to read from the pipe");

                if (_client && !_pluginHostInitialized)
                {
                    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Plugin is not initialized, exiting MessageHandlerWorker");
                }
                return;
            }

            if (incomingMessage->messageType == Request)
            {
                TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Recieved a Request Message");
                _request.Push(incomingMessage);
                SetEvent(_hRequestNotification);
                TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Request notification sent");
            }
            else if (incomingMessage->messageType == Response)
            {
                TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Recieved a Response Message");
                _response.Push(incomingMessage);
                SetEvent(_hResponseNotification);
                TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Response notification sent");
            }
        }
    }

    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Exiting message handler thread");
}

void PluginNamedPipeTransport::ProcessRequest()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    bool bProcessRequests = true;
    while (bProcessRequests)
    {
        HANDLE hEvents[2];
        hEvents[0] = _hRequestNotification;
        hEvents[1] = _hShutdownNotification;
        switch(WaitForMultipleObjects(2, hEvents, FALSE, INFINITE))
        {
            case WAIT_OBJECT_0:
            {
                if (_client && !_pluginHostInitialized)
                {
                    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Plugin is not initialized, exiting MessageHandlerWorker");
                    return;
                }

                TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Message Processing Thread: Received new request notification");
                std::shared_ptr<Message> requestMessage = _request.Pop();
                if (requestMessage)
                {
                    ProcessRequestMessage(requestMessage);
                }
                break;
            }
            case WAIT_OBJECT_0 + 1:
            default:
            {
                bProcessRequests = false;
            }
        }
    }
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Message Processing Thread: Exiting");
}

void PluginNamedPipeTransport::ProcessRequestMessage(std::shared_ptr<Message> requestMessage)
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    std::shared_ptr<Message> response = std::make_shared<Message>();
    response->messageType = Response;
    response->callType = None;
    response->errorCode = DM_ERROR_SUCCESS;

    char *responseData;
    if (_client && _pluginHostInitialized)
    {
        // if client, this is a reverse invoke call,
        assert(requestMessage->callType == ReverseInvokeCall);

        TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Received reverse-invoke call. Calling reverse invoke...");
        response->errorCode = _pluginReverseInvoke((char*)requestMessage->data, &responseData);

        TRACELINEP(LoggingLevel::Verbose, "[Transport Layer] " "Reverse invoke returned with error code ", response->errorCode);
        // Copy response data
        std::memcpy(response->data, responseData, strlen(responseData) + 1);
        // Freeup the buffer
        _pluginReverseDeleteBuffer(responseData);
        TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Deleted ReverseInvoke buffer");
    }

    // For plugin
    if (!_client)
    {
        if (requestMessage->callType == PluginInvokeCall)
        {
            response->errorCode = _pluginInvoke((char*)requestMessage->data, &responseData);
            // Copy response data
            std::memcpy(response->data, responseData, strlen(responseData) + 1);
            // Freeup the buffer
            _pluginDeleteBuffer(responseData);
        }
        else if (requestMessage->callType == PluginCreateCall)
        {
            response->errorCode = _pluginCreate();
            if (response->errorCode == DM_ERROR_SUCCESS)
            {
                size_t responseLength = strlen(PLUGIN_CREATE_SUCCESS_RESPONSE) + 1;
                std::memcpy(response->data, PLUGIN_CREATE_SUCCESS_RESPONSE, responseLength);
            }
            else
            {
                size_t responseLength = strlen(PLUGIN_CREATE_FAILED_RESPONSE) + 1;
                std::memcpy(response->data, PLUGIN_CREATE_FAILED_RESPONSE, responseLength);
            }
        }
        else if (requestMessage->callType == PluginShutdownCall)
        {
            TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Received plugin shutdown call");

            // Close the transport
            CloseTransport();
            TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Transport Closed");

            // Signal shutdown
            SetEvent(_hShutdownNotification);
        }
    }

    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Sending response");

    // if client, write to pluginPipe or if plugin write to client pipe
    HANDLE hPipe = _client ? _hPluginPipe : _hClientPipe;

    DWORD cbWritten = 0;
    DWORD cbResponseSize = sizeof(Message);
    // Write the reply to the pipe.
    BOOL fSuccess = WriteFile(
        hPipe,          // handle to pipe
        response.get(), // buffer to write from
        cbResponseSize, // number of bytes to write
        &cbWritten,     // number of bytes written
        nullptr);       // not overlapped I/O

    if (!fSuccess || cbResponseSize != cbWritten)
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to write to named pipe");
    }

    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Response sent");
}

void PluginNamedPipeTransport::CloseTransport()
{
    TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

    // Set the shutdown event
    SetEvent(_hShutdownNotification);

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

    if (_client && _hShutdownNotification != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hShutdownNotification);
        _hShutdownNotification = INVALID_HANDLE_VALUE;
    }
}

}}}}
