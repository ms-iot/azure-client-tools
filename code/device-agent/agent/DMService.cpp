// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DMConstants.h"
#include "DMService.h"
#include "AzureDMClient.h"

using namespace std;
using namespace std::chrono;
using namespace std::experimental;
using namespace Microsoft::Azure::DeviceManagement::Utils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

const unsigned int RecoverInterval = 60;   // in seconds

DMService *DMService::s_service = NULL;

void DMService::RunService()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    s_service = this;

    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { const_cast<LPWSTR>(_name.c_str()), ServiceMain },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcher(serviceTable))
    {
        TRACELINEP(LoggingLevel::Error, "Failed to start the service. Error: ", GetLastError());
    }
}

void WINAPI DMService::ServiceMain(DWORD, PWSTR*)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    assert(s_service != NULL);

    s_service->_statusHandle = RegisterServiceCtrlHandler(s_service->_name.c_str(), ServiceCtrlHandler);
    if (s_service->_statusHandle == NULL)
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to register service control handler");
    }

    s_service->Start();
}

void WINAPI DMService::ServiceCtrlHandler(DWORD ctrl)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    switch (ctrl)
    {
    case SERVICE_CONTROL_STOP:
        TRACELINE(LoggingLevel::Verbose, "Service stop received...");
        s_service->Stop();
        break;
    case SERVICE_CONTROL_PAUSE:
        TRACELINE(LoggingLevel::Verbose, "Service pause received...");
        s_service->Pause();
        break;
    case SERVICE_CONTROL_CONTINUE:
        TRACELINE(LoggingLevel::Verbose, "Service continue received...");
        s_service->Continue();
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        TRACELINE(LoggingLevel::Verbose, "Service shutdown received...");
        s_service->Shutdown();
        break;
    case SERVICE_CONTROL_INTERROGATE:
        TRACELINE(LoggingLevel::Verbose, "Service interrogate received...");
        break;
    default: break;
    }
}

DMService::DMService(
    const wstring& serviceName,
    shared_ptr<ServiceParameters> serviceParameters) :
    _serviceParameters(serviceParameters)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    assert(serviceName.size() != 0);

    _name = serviceName;
    _statusHandle = NULL;

    _status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    _status.dwCurrentState = SERVICE_START_PENDING;

    DWORD dwControlsAccepted = 0;
    dwControlsAccepted |= SERVICE_ACCEPT_STOP;
    dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
    dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
    _status.dwControlsAccepted = dwControlsAccepted;

    _status.dwWin32ExitCode = NO_ERROR;
    _status.dwServiceSpecificExitCode = 0;
    _status.dwCheckPoint = 0;
    _status.dwWaitHint = 0;

    s_service = this;
}

void DMService::Start()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    EnsureReported([this]() {
        SetServiceStatus(SERVICE_START_PENDING);
        OnStart();
        SetServiceStatus(SERVICE_RUNNING);
    });
}

void DMService::Stop()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    EnsureReported([this]() {
        SetServiceStatus(SERVICE_STOP_PENDING);
        OnStop();
        SetServiceStatus(SERVICE_STOPPED);
    });
}

void DMService::Pause()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    EnsureReported([this]() {
        SetServiceStatus(SERVICE_PAUSE_PENDING);
        SetServiceStatus(SERVICE_PAUSED);
    });
}

void DMService::Continue()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    EnsureReported([this]() {
        SetServiceStatus(SERVICE_CONTINUE_PENDING);
        SetServiceStatus(SERVICE_RUNNING);
    });
}

void DMService::Shutdown()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    EnsureReported([this]() {
        SetServiceStatus(SERVICE_STOPPED);
    });
}

void DMService::SetServiceStatus(DWORD currentState, DWORD win32ExitCode)
{
    static DWORD checkPoint = 1;

    _status.dwCurrentState = currentState;
    _status.dwWin32ExitCode = win32ExitCode;
    _status.dwWaitHint = 0;
    _status.dwCheckPoint = ((currentState == SERVICE_RUNNING) || (currentState == SERVICE_STOPPED)) ? 0 : checkPoint++;

    if (!::SetServiceStatus(_statusHandle, &_status))
    {
        WriteErrorLogEntry(L"Error: Could not set service status. Error Code ", GetLastError());
    }
}

void DMService::WriteEventLogEntry(const wstring& message, WORD type)
{
    TRACELINE(LoggingLevel::Verbose, message.c_str());

    HANDLE eventSource = RegisterEventSource(NULL, _name.c_str());
    if (!eventSource)
    {
        TRACELINEP(LoggingLevel::Verbose, L"Error: Failed to register the event source. Error Code ", GetLastError());
        // No exception is thrown since it is not a scenario that should affect normal operations.
        return;
    }

    LPCWSTR strings[2] = {
        _name.c_str(),
        message.c_str()
    };

    if (!ReportEvent(eventSource,   // Event log handle
        type,                  // Event type
        0,                     // Event category
        0,                     // Event identifier
        NULL,                  // No security identifier
        2,                     // Size of strings array
        0,                     // No binary data
        strings,               // Array of strings
        NULL                   // No binary data
    ))
    {
        TRACELINEP(LoggingLevel::Verbose, L"Error: Failed to write to the event log. Error Code ", GetLastError());
        // No exception is thrown since it is not a scenario that should affect normal operations.
    }

    if (!DeregisterEventSource(eventSource))
    {
        TRACELINEP(LoggingLevel::Verbose, L"Error: Failed to unregister the event source. Error Code ", GetLastError());
        // No exception is thrown since it is not a scenario that should affect normal operations.
    }
}

void DMService::EnsureReported(const function<void()>& Action)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    try
    {
        Action();
    }
    catch (const DMException& e)
    {
        TRACELINE(LoggingLevel::Error, e.DisplayMessage().c_str());
    }
    catch (const exception& e)
    {
        TRACELINE(LoggingLevel::Error, e.what());
    }
}

void DMService::WriteErrorLogEntry(const wstring& function, DWORD errorCode)
{
    basic_ostringstream<wchar_t> message;
    message << function << L" failed w/err " << errorCode;
    WriteEventLogEntry(message.str(), EVENTLOG_ERROR_TYPE);
}

void DMService::OnStart()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    _workerThread = thread(ServiceWorkerThread, this);
}

void DMService::ServiceWorkerThread(void* context)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    /*
    CreateTimerQueueTimer(
        &_temporaryFilesCleanupTimer,
        NULL,                                   // default timer queue
        CleanupTemporaryFiles,
        this,
        0,                                      // start immediately
        1000 * 60 * 60 * 24,                    // every day
        WT_EXECUTEDEFAULT);
    */

    DMService* iotDMService = static_cast<DMService*>(context);
    iotDMService->RunClient();

    TRACELINE(LoggingLevel::Verbose, "Worker thread exiting...");
}

void DMService::RunClient()
{
    _stopEvent.SetHandle(CreateEvent(nullptr, TRUE /*manual reset*/, FALSE /*not set*/, nullptr));
    if (_stopEvent.Get() == NULL)
    {
        TRACELINEP(LoggingLevel::Error, "CreateEvent() failed. Error: ", GetLastError());
        return;
    }

    bool failureOccured = true;
    do
    {
        EnsureReported([this, &failureOccured]() {

            shared_ptr<AzureDMClient> azureClient = AzureDMClient::Create();
            azureClient->Run(_serviceParameters, _stopEvent.Get());
            failureOccured = false;
        });

        // If we're done and no failures have occured, exit the loop...
        if (!failureOccured)
            break;

        // Otherwise, wait a little, and re-try...
        ::Sleep(RecoverInterval * 1000);

    } while (true);
}

static VOID CALLBACK CleanupTemporaryFiles(PVOID /*ParameterPtr*/, BOOLEAN)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // handle garbage collection
}

void DMService::ServiceWorkerThreadHelper(void)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    CreateTimerQueueTimer(
        &_temporaryFilesCleanupTimer,
        NULL,                                   // default timer queue
        CleanupTemporaryFiles,
        this,
        0,                                      // start immediately
        1000 * 60 * 60 * 24,                    // every day
        WT_EXECUTEDEFAULT);

    for (unsigned int i = 0; i < 100; ++i)
    {
        ::Sleep(1000);
        TRACELINE(LoggingLevel::Verbose, L"Working...");
    }

    TRACELINE(LoggingLevel::Verbose, "Worker thread exiting.");
}

void DMService::OnStop()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    SetEvent(_stopEvent.Get());
}

void DMService::Install(
    const wstring& serviceName,
    const wstring& displayName,
    DWORD startType,
    const wstring& dependencies,
    const wstring& account,
    const wstring& password)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    wchar_t szPath[MAX_PATH];
    if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0)
    {
        TRACELINEP(LoggingLevel::Verbose, L"GetModuleFileName failed w/err :", GetLastError());
        return;
    }

    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL)
    {
        TRACELINEP(LoggingLevel::Verbose, L"OpenSCManager failed w/err :", GetLastError());
        return;
    }

    // Install the service into SCM by calling CreateService
    SC_HANDLE schService = CreateService(
        schSCManager,                   // SCManager database
        serviceName.c_str(),            // Name of service
        displayName.c_str(),            // Name to display
        SERVICE_QUERY_STATUS,           // Desired access
        SERVICE_WIN32_OWN_PROCESS,      // Service type
        startType,                      // Service start type
        SERVICE_ERROR_NORMAL,           // Error control type
        szPath,                         // Service's binary
        NULL,                           // No load ordering group
        NULL,                           // No tag identifier
        dependencies.c_str(),           // Dependencies
        account.c_str(),                // Service running account
        password.c_str()                // Password of the account
    );

    if (schService != NULL)
    {
        CloseServiceHandle(schService);
        schService = NULL;
        TRACELINE(LoggingLevel::Verbose, L"Installed successfully!");
    }
    else
    {
        TRACELINEP(LoggingLevel::Verbose, L"CreateService failed w/err :", GetLastError());
    }

    CloseServiceHandle(schSCManager);
    schSCManager = NULL;
}

void DMService::Uninstall(const wstring& serviceName)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        TRACELINEP(LoggingLevel::Verbose, L"OpenSCManager failed w/err :", GetLastError());
        return;
    }

    // Open the service with delete, stop, and query status permissions
    SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
    if (schService != NULL)
    {
        // Try to stop the service
        SERVICE_STATUS ssSvcStatus = {};
        if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
        {
            TRACELINEP(LoggingLevel::Verbose, L"Stopping ", serviceName);
            Sleep(1000);

            while (QueryServiceStatus(schService, &ssSvcStatus))
            {
                if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
                {
                    TRACELINE(LoggingLevel::Verbose, L".");
                    Sleep(1000);
                }
                else
                {
                    break;
                }
            }

            if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
            {
                TRACELINE(LoggingLevel::Verbose, L"\nStopped.");
            }
            else
            {
                TRACELINE(LoggingLevel::Verbose, L"\nError: Failed to stop.");
            }
        }

        // Now remove the service by calling DeleteService.
        if (DeleteService(schService))
        {
            TRACELINE(LoggingLevel::Verbose, L"Service uninstalled.");
        }
        else
        {
            TRACELINEP(LoggingLevel::Verbose, L"DeleteService failed w/err :", GetLastError());
        }

        CloseServiceHandle(schService);
        schService = NULL;
    }
    else
    {
        TRACELINEP(LoggingLevel::Verbose, L"OpenService failed w/err :", GetLastError());
    }

    CloseServiceHandle(schSCManager);
    schSCManager = NULL;
}

}}}}
