// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <iomanip>
#include <experimental/filesystem>

#include "PluginNamedPipePluginTransport.h"
#include "PluginConstants.h"

using namespace std;
using namespace experimental::filesystem;
using namespace experimental::filesystem::v1;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    PluginNamedPipePluginTransport::PluginNamedPipePluginTransport(
        const wstring& pluginPath,
        const wstring& clientPipeName,
        const wstring& pluginPipeName) :
        PluginNamedPipeBaseTransport(clientPipeName, pluginPipeName),
        _pluginCreate(nullptr),
        _pluginInvoke(nullptr),
        _pluginDeleteBuffer(nullptr)
    {
        _debugIdentity = path(DMUtils::WideToMultibyte(pluginPath.c_str())).filename().string();
    }

    PluginNamedPipePluginTransport::~PluginNamedPipePluginTransport()
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    }

    void PluginNamedPipePluginTransport::Initialize()
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        InitializeDequeEvents();

        std::wstring clientPipeFullName = L"\\\\.\\pipe\\" + _clientPipeName;
        std::wstring pluginPipeFullName = L"\\\\.\\pipe\\" + _pluginPipeName;

        // Open the named pipes
        _hWritePipe = CreateFile(
            clientPipeFullName.c_str(),   // pipe name
            GENERIC_WRITE,                // write access (plugin->client)
            0,                            // no sharing
            nullptr,                      // default security attributes
            OPEN_EXISTING,                // opens existing pipe
            0,                            // default attributes
            nullptr);                     // no template file

        if (_hWritePipe == INVALID_HANDLE_VALUE)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to open client named pipe");
        }

        DWORD dwMode = PIPE_READMODE_MESSAGE;
        if (!SetNamedPipeHandleState(_hWritePipe, &dwMode, nullptr, nullptr))
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to set the pipe readmode for client pipe");
        }

        _hReadPipe = CreateFile(
            pluginPipeFullName.c_str(),   // pipe name
            GENERIC_READ,                 // read access (client->plugin)
            0,                            // no sharing
            nullptr,                      // default security attributes
            OPEN_EXISTING,                // opens existing pipe
            0,                            // default attributes
            nullptr);                     // no template file

        if (_hReadPipe == INVALID_HANDLE_VALUE)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to open plugin named pipe");
        }

        // Call common initialization.
        RestartMonitorThreads();
    }

    void PluginNamedPipePluginTransport::ProcessRequestMessage(
        std::shared_ptr<Message> requestMessage)
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        std::shared_ptr<Message> response = std::make_shared<Message>();
        response->messageType = Response;
        response->callType = None;
        response->errorCode = DM_ERROR_SUCCESS;

        char *responseData = nullptr;
        if (requestMessage->callType == PluginInvokeCall)
        {
            response->errorCode = _pluginInvoke(requestMessage->Payload(), &responseData);
            // Copy response data
            response->SetData(responseData, strlen(responseData) + 1);
            // Freeup the buffer
            _pluginDeleteBuffer(responseData);
        }
        else if (requestMessage->callType == PluginCreateCall)
        {
            response->errorCode = _pluginCreate();
            if (response->errorCode == DM_ERROR_SUCCESS)
            {
                size_t responseLength = strlen(PLUGIN_CREATE_SUCCESS_RESPONSE) + 1;
                response->SetData(PLUGIN_CREATE_SUCCESS_RESPONSE, responseLength);
            }
            else
            {
                size_t responseLength = strlen(PLUGIN_CREATE_FAILED_RESPONSE) + 1;
                response->SetData(PLUGIN_CREATE_FAILED_RESPONSE, responseLength);
            }
        }
        else if (requestMessage->callType == PluginShutdownCall)
        {
            TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Received plugin shutdown call");

            // Close the transport
            CloseTransport();
            TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Transport Closed");

            // Signal shutdown
            SetEvent(_hShutdownNotification);
        }

        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "Sending response");

        WriteMessage(response);

        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " "Response sent");
    }

    void PluginNamedPipePluginTransport::SetClientInterface(
        PluginReverseInvokePtr reverseInvokePtr,
        PluginReverseDeleteBufferPtr reverseDeletePtr)
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);
        throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_OPERATION, "Set reverse invoke should only be called by client");
    }

    void PluginNamedPipePluginTransport::SetPluginInterface(
        PluginCreatePtr createPtr,
        PluginInvokePtr invokePtr,
        PluginDeleteBufferPtr deleteBufferPtr)
    {
        TRANSPORT_TRACELINE(LoggingLevel::Verbose, "[Transport Layer] " __FUNCTION__);

        _pluginCreate = createPtr;
        _pluginInvoke = invokePtr;
        _pluginDeleteBuffer = deleteBufferPtr;
    }

}}}}
