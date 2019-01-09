// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <thread>
#include <atomic>
#include "..\Utilities\Utils.h"
#include "..\Utilities\AutoCloseHandle.h"
#include "ServiceParameters.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

class DMService
{
public:

    DMService(
        const std::wstring& serviceName,
        std::shared_ptr<ServiceParameters> serviceParameters);

    static void Install(
        const std::wstring&  serviceName,
        const std::wstring&  displayName,
        DWORD startType,
        const std::wstring&  dependencies,
        const std::wstring&  account,
        const std::wstring&  password);

    static void Uninstall(const std::wstring&  serviceName);

    void RunService();

    void RunClient();

private:

    // Methods
    static void WINAPI ServiceMain(DWORD argc, LPWSTR *argv);
    static void WINAPI ServiceCtrlHandler(DWORD ctrl);

    static void ServiceWorkerThread(void* context);
    void ServiceWorkerThreadHelper();

    void Start();
    void Stop();
    void Pause();
    void Continue();
    void Shutdown();

    virtual void OnStart();
    virtual void OnStop();

    void DisableEnqueue();

    // Helpers
    void EnsureReported(const std::function<void()>& Action);

    void SetServiceStatus(DWORD currentState, DWORD win32ExitCode = NO_ERROR);

    void WriteEventLogEntry(const std::wstring& message, WORD type);
    void WriteErrorLogEntry(const std::wstring& function, DWORD errorCode = GetLastError());

    // Data members
    static DMService* s_service;    // ToDo: why is this a pointer?

    std::wstring _name;
    SERVICE_STATUS _status;
    SERVICE_STATUS_HANDLE _statusHandle;

    // timer for cleaning up temporary files
    HANDLE _temporaryFilesCleanupTimer;

    std::shared_ptr<ServiceParameters> _serviceParameters;

    // threads
    Microsoft::Azure::DeviceManagement::Utils::JoiningThread _workerThread;

    // Synchronization between worker thread and main thread for exiting...
    Microsoft::Azure::DeviceManagement::Utils::AutoCloseHandle _stopEvent;
};

}}}}
