// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <functional>
#include <vector>
#include "ServiceManager.h"
#include "..\Utilities\Utils.h"
#include "..\Utilities\AutoCloseTypes.h"
#include "..\Utilities\AutoCloseHandle.h"

using namespace std;
using namespace Microsoft::Azure::DeviceManagement::Utils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

DWORD ServiceManager::GetStatus(const std::wstring& serviceName)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    TRACELINEP(LoggingLevel::Verbose, L"Checking the running state of service: ", serviceName.c_str());

    AutoCloseServiceHandle serviceManagerHandle = OpenSCManager(NULL /*local machine*/, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
    if (serviceManagerHandle.Get() == NULL)
    {
        throw DMException(GetLastError(), "OpenSCManager() failed.");
    }

    AutoCloseServiceHandle serviceHandle = OpenService(serviceManagerHandle.Get() /*scm manager*/, serviceName.c_str(), SERVICE_ALL_ACCESS);
    if (serviceHandle.Get() == NULL)
    {
        throw DMException(GetLastError(), "OpenService() failed.");
    }

    SERVICE_STATUS serviceStatus;
    if (!QueryServiceStatus(serviceHandle.Get() /*service handle*/, &serviceStatus))
    {
        throw DMException(GetLastError(), "QueryServiceStatus() failed.");
    }

    return serviceStatus.dwCurrentState;
}

void ServiceManager::WaitStatus(const wstring& serviceName, DWORD status, unsigned int maxWaitInSeconds)
{
    while (maxWaitInSeconds--)
    {
        if (status == ServiceManager::GetStatus(serviceName))
        {
            break;
        }
        TRACELINEP(LoggingLevel::Verbose, L"Waiting for service: ", serviceName.c_str());
        ::Sleep(1000);
        --maxWaitInSeconds;
    }
}

DWORD ServiceManager::GetStartType(const std::wstring& serviceName)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    TRACELINEP(LoggingLevel::Verbose, L"Checking the enabled state of service: ", serviceName.c_str());

    AutoCloseServiceHandle serviceManagerHandle = OpenSCManager(NULL /*local machine*/, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
    if (serviceManagerHandle.Get() == NULL)
    {
        throw DMException(GetLastError(), "OpenSCManager() failed.");
    }

    AutoCloseServiceHandle serviceHandle = OpenService(serviceManagerHandle.Get() /*scm manager*/, serviceName.c_str(), SERVICE_ALL_ACCESS);
    if (serviceHandle.Get() == NULL)
    {
        throw DMException(GetLastError(), "OpenService() failed.");
    }

    DWORD bytesNeeded = 0;
    if (!QueryServiceConfig(serviceHandle.Get(), NULL, 0, &bytesNeeded) && (ERROR_INSUFFICIENT_BUFFER != GetLastError()))
    {
        throw DMException(GetLastError(), "QueryServiceConfig() failed.");
    }

    vector<char> buffer(bytesNeeded);
    if (!QueryServiceConfig(serviceHandle.Get(), reinterpret_cast<QUERY_SERVICE_CONFIG*>(buffer.data()), static_cast<DWORD>(buffer.size()), &bytesNeeded))
    {
        throw DMException(GetLastError(), "QueryServiceConfig() failed.");
    }

    QUERY_SERVICE_CONFIG* config = reinterpret_cast<QUERY_SERVICE_CONFIG*>(buffer.data());
    return config->dwStartType;
}

void ServiceManager::StartStop(const std::wstring& serviceName, bool start)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    TRACELINEP(LoggingLevel::Verbose, L"Starting service: ", serviceName.c_str());

    AutoCloseServiceHandle serviceManagerHandle = OpenSCManager(NULL /*local machine*/, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
    if (serviceManagerHandle.Get() == NULL)
    {
        throw DMException(GetLastError(), "OpenSCManager() failed.");
    }

    AutoCloseServiceHandle serviceHandle = OpenService(serviceManagerHandle.Get() /*scm manager*/, serviceName.c_str(), SERVICE_ALL_ACCESS);
    if (serviceHandle.Get() == NULL)
    {
        throw DMException(GetLastError(), "OpenService() failed.");
    }

    SERVICE_STATUS serviceStatus;
    if (!QueryServiceStatus(serviceHandle.Get() /*service handle*/, &serviceStatus))
    {
        throw DMException(GetLastError(), "QueryServiceStatus() failed.");
    }

    if (start)
    {
        // Start...
        if (serviceStatus.dwCurrentState == SERVICE_RUNNING)
        {
            TRACELINE(LoggingLevel::Verbose, L"Service is already running!");
            return;
        }

        if (!StartService(serviceHandle.Get() /*service handle*/, 0 /* arg count*/, NULL /* no args*/))
        {
            throw DMException(GetLastError(), "StartService() failed.");
        }
        TRACELINE(LoggingLevel::Verbose, L"Service has been started successfully");
    }
    else
    {
        // Stop...
        if (serviceStatus.dwCurrentState != SERVICE_RUNNING)
        {
            TRACELINE(LoggingLevel::Verbose, L"Service is already stopped!");
            return;
        }

        if (!ControlService(serviceHandle.Get() /*service handle*/, SERVICE_CONTROL_STOP, &serviceStatus))
        {
            throw DMException(GetLastError(), "ControlService() failed.");
        }
        TRACELINE(LoggingLevel::Verbose, L"Service has been stopped successfully");
    }
}

void ServiceManager::Start(const std::wstring& serviceName)
{
    StartStop(serviceName, true /*start*/);
}

void ServiceManager::Stop(const std::wstring& serviceName)
{
    StartStop(serviceName, false /*stop*/);
}

void ServiceManager::SetStartType(const std::wstring& serviceName, DWORD startType)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    TRACELINEP(LoggingLevel::Verbose, L"Enabling auto startup for service: ", serviceName.c_str());

    AutoCloseServiceHandle serviceManagerHandle = OpenSCManager(NULL /*local machine*/, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
    if (serviceManagerHandle.Get() == NULL)
    {
        throw DMException(GetLastError(), "OpenSCManager() failed.");
    }

    AutoCloseServiceHandle serviceHandle = OpenService(serviceManagerHandle.Get() /*scm manager*/, serviceName.c_str(), SERVICE_ALL_ACCESS);
    if (serviceHandle.Get() == NULL)
    {
        throw DMException(GetLastError(), "OpenService() failed.");
    }

    if (!ChangeServiceConfig(serviceHandle.Get(),
        SERVICE_NO_CHANGE,
        startType,
        SERVICE_NO_CHANGE,
        NULL, /*path not changing*/
        NULL, /*load order group not changing*/
        NULL, /*TagIId not changing*/
        NULL, /*dependencies not changing*/
        NULL, /*account nmae not changing*/
        NULL, /*password not changing*/
        NULL)) /*display name not changing*/
    {
        throw DMException(GetLastError(), "ChangeServiceConfig() to change service to auto start.");
    }
}

}}}}
