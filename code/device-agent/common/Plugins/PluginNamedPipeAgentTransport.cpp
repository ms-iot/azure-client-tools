// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <iomanip>
#include <experimental/filesystem>

#include "PluginNamedPipeAgentTransport.h"

#define PLUGIN_SHUTDOWN_TIME 5000

using namespace std;
using namespace experimental::filesystem;
using namespace experimental::filesystem::v1;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    PluginNamedPipeAgentTransport::PluginNamedPipeAgentTransport(
        const std::wstring& pluginPath,
        long keepAliveTime,
        const std::wstring& clientPipeName,
        const std::wstring& pluginPipeName,
        IPluginStateMonitor* pluginStateMonitor) :
        PluginNamedPipeBaseTransport(clientPipeName, pluginPipeName),
        _keepAliveTime(keepAliveTime),
        _pluginHostInitialized(false),
        _pluginPath(pluginPath),
        _pluginStateMonitor(pluginStateMonitor),
        _pluginReverseInvoke(nullptr),
        _pluginReverseDeleteBuffer(nullptr)
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        _debugIdentity = path(DMUtils::WideToMultibyte(pluginPath.c_str())).filename().string();
    }

    PluginNamedPipeAgentTransport::~PluginNamedPipeAgentTransport()
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    }

    void PluginNamedPipeAgentTransport::LaunchPluginHost()
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

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
        cmdLine += L" -logsPath <replace with logs path>";
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

        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Pluginhost has been launched.");

        // Create a process monitoring thread
        _pluginHostMonitoringThread.Join();

        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Spawning PluginHostMonitoringThreadProc().");
        _pluginHostMonitoringThread = std::thread(PluginNamedPipeAgentTransport::PluginHostMonitoringThreadProc, this);
    }

    void PluginNamedPipeAgentTransport::Initialize()
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        if (_pluginHostInitialized)
        {
            TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Plugin already initialized.");
            return;
        }

        InitializeDequeEvents();

        std::wstring clientPipeFullName = L"\\\\.\\pipe\\" + _clientPipeName;
        std::wstring pluginPipeFullName = L"\\\\.\\pipe\\" + _pluginPipeName;

        // Create NamedPipe
        _hReadPipe = CreateNamedPipe(
            clientPipeFullName.c_str(),         // pipe name
            PIPE_ACCESS_DUPLEX,                 // Inbound plugin->client
            PIPE_TYPE_MESSAGE |                 // message type pipe
            PIPE_READMODE_MESSAGE |             // message-read mode
            PIPE_WAIT,                          // blocking mode
            PIPE_UNLIMITED_INSTANCES,           // max. instances
            IPC_BUFFER_SIZE,                    // output buffer size
            IPC_BUFFER_SIZE,                    // input buffer size
            0,                                  // client time-out
            nullptr);                           // default security attribute

        if (_hReadPipe == INVALID_HANDLE_VALUE)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Unable to open client named pipe");
        }

        _hWritePipe = CreateNamedPipe(
            pluginPipeFullName.c_str(), // pipe name
            PIPE_ACCESS_DUPLEX,                 // Outbound client->plugin
            PIPE_TYPE_MESSAGE |                 // message type pipe
            PIPE_READMODE_MESSAGE |             // message-read mode
            PIPE_WAIT,                          // blocking mode
            PIPE_UNLIMITED_INSTANCES,           // max. instances
            IPC_BUFFER_SIZE,                    // output buffer size
            IPC_BUFFER_SIZE,                    // input buffer size
            0,                                  // client time-out
            nullptr);                           // default security attribute

        if (_hWritePipe == INVALID_HANDLE_VALUE)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to create plugin named pipe");
        }

        DWORD dwMode = PIPE_READMODE_MESSAGE;
        if (!SetNamedPipeHandleState(_hReadPipe, &dwMode, nullptr, nullptr))
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "SetNamedPipeHandleState failed for client pipe");
        }

        if (!SetNamedPipeHandleState(_hWritePipe, &dwMode, nullptr, nullptr))
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "SetNamedPipeHandleState failed for plugin pipe");
        }

        // Launch the plugin host
        LaunchPluginHost();

        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Connecting to client pipe...");
        ConnectNamedPipe(_hReadPipe, nullptr);

        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Connecting to plugin pipe...");
        ConnectNamedPipe(_hWritePipe, nullptr);

        // Reset the events
        ResetEvent(_hRequestNotification);
        ResetEvent(_hResponseNotification);
        ResetEvent(_hShutdownNotification);

        // Clear the queues
        _requestQueue.Clear();
        _responseQueue.Clear();

        _pluginHostInitialized = true;

        // Call common initialization.
        RestartMonitorThreads();

        // Notify listeners...
        if (_pluginStateMonitor)
        {
            _pluginStateMonitor->OnPluginLoaded();
        }
    }

    void PluginNamedPipeAgentTransport::SetClientInterface(
        PluginReverseInvokePtr reverseInvokePtr,
        PluginReverseDeleteBufferPtr reverseDeletePtr)
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
        _pluginReverseInvoke = reverseInvokePtr;
        _pluginReverseDeleteBuffer = reverseDeletePtr;
    }

    void PluginNamedPipeAgentTransport::SetPluginInterface(
        PluginCreatePtr createPtr,
        PluginInvokePtr invokePtr,
        PluginDeleteBufferPtr deleteBufferPtr)
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
        throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_OPERATION, "Set plugin interface should only be called by the plugin");
    }

    std::shared_ptr<Message> PluginNamedPipeAgentTransport::SendAndGetResponse(std::shared_ptr<Message> message)
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        lock_guard<recursive_mutex> guard(_mutex);

        // If plugin is not initialized, initialize it first
        if (!_pluginHostInitialized || _hWritePipe == INVALID_HANDLE_VALUE)
        {
            Initialize();
        }

        return PluginNamedPipeBaseTransport::SendAndGetResponse(message);
    }

    void PluginNamedPipeAgentTransport::ProcessRequestMessage(std::shared_ptr<Message> requestMessage)
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        std::shared_ptr<Message> response = std::make_shared<Message>();
        response->messageType = Response;
        response->callType = None;
        response->errorCode = DM_ERROR_SUCCESS;

        char *responseData;
        if (_pluginHostInitialized)
        {
            // if client, this is a reverse invoke call,
            assert(requestMessage->callType == ReverseInvokeCall);

            TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Received reverse-invoke call. Calling reverse invoke...");
            response->errorCode = _pluginReverseInvoke(requestMessage->Payload(), &responseData);

            TRANSPORT_TRACELINEP(LoggingLevel::Verbose, "[Transport Layer] " "Reverse invoke returned with error code ", response->errorCode);
            // Copy response data
            response->SetData(responseData, strlen(responseData) + 1);
            // Freeup the buffer
            _pluginReverseDeleteBuffer(responseData);
            TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Deleted ReverseInvoke buffer");
        }

        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Sending response");

        WriteMessage(response);

        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Response sent");
    }

    void PluginNamedPipeAgentTransport::SendShutdownToPluginHost()
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        std::lock_guard<std::recursive_mutex> guard(_mutex);

        // Timeout reached or received shutdown notification send a shutdown call and wait for process to exit
        std::shared_ptr<Message> shutdownMessage = std::make_shared<Message>();
        shutdownMessage->messageType = Request;
        shutdownMessage->callType = PluginShutdownCall;
        shutdownMessage->errorCode = DM_ERROR_SUCCESS;

        DWORD cbWritten = 0;
        DWORD cbMessageSize = sizeof(Message);

        BOOL fSuccess = WriteFile(
            _hWritePipe,           // handle to pipe
            shutdownMessage.get(), // buffer to write from
            cbMessageSize,         // number of bytes to write
            &cbWritten,            // number of bytes written
            nullptr);              // not overlapped I/O

        if (!fSuccess || cbMessageSize != cbWritten)
        {
            TRANSPORT_TRACELINE(LoggingLevel::Error, "Failed to send shutdown message to the plugin");
            TerminateProcess(_processInfo.hProcess, -1);
        }

        // Wait for 5 seconds for process to exit
        TRANSPORT_TRACELINE(LoggingLevel::Error, "Waiting 5 seconds for plugin host to exit.");
        DWORD exitCode = -1;
        if (WAIT_OBJECT_0 == WaitForSingleObject(_processInfo.hProcess, PLUGIN_SHUTDOWN_TIME))
        {
            exitCode = 0;
        }
        TerminateProcess(_processInfo.hProcess, exitCode);
    }

    void PluginNamedPipeAgentTransport::RunPluginHostMonitor()
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        if (_processInfo.hProcess == nullptr)
        {
            // There's nothing to monitor
            return;
        }

        try
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
                TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Plugin host exited. Shutting down transport.");
                CloseHandle(_processInfo.hProcess);
                ZeroMemory(&_processInfo, sizeof(PROCESS_INFORMATION));

                _pluginHostInitialized = false;

                CloseTransport();
                break;
            }
            case WAIT_OBJECT_0 + 1:
            {
                TRANSPORT_TRACELINE(LoggingLevel::Verbose, "A shutdown notification received. Sending plugin host a request to exit.");

                SendShutdownToPluginHost();

                // CloseTransport() is not needed in this case, because shutdown signal will take care of triggering the right 
                // clean-up code.

                _pluginHostInitialized = false;

                break;
            }
            case WAIT_TIMEOUT:
            {
                TRANSPORT_TRACELINE(LoggingLevel::Verbose, "A Timeout or Shutdown notification received. Sending plugin host a request to exit.");

                SendShutdownToPluginHost();

                CloseTransport();

                _pluginHostInitialized = false;
                break;
            }
            default:
            {
                TRANSPORT_TRACELINE(LoggingLevel::Error, "Wait for process handle failed.");
                break;
            }
            }
        }
        catch (const DMException& ex)
        {
            LogDMException(ex, "An error occured while processing Plugin Host Monitor thread.", "");
        }
        catch (const exception& ex)
        {
            LogStdException(ex, "An error occured while processing Plugin Host Monitor thread.", "");
        }
        TRANSPORT_TRACELINE(LoggingLevel::Error, "Exiting plugin host monitoring thread.");
    }

    void PluginNamedPipeAgentTransport::PluginHostMonitoringThreadProc(void* context)
    {
        PluginNamedPipeAgentTransport* transport = static_cast<PluginNamedPipeAgentTransport*>(context);
        transport->RunPluginHostMonitor();
    }

    void PluginNamedPipeAgentTransport::StopMonitorThreads()
    {
        PluginNamedPipeBaseTransport::StopMonitorThreads();

        // Cannot join self...
        if (this_thread::get_id() != _pluginHostMonitoringThread.GetId())
        {
            _pluginHostMonitoringThread.Join();
        }
    }

}}}}
