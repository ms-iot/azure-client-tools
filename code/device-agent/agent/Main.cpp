// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <iostream>
#include "ServiceParameters.h"
#include "DMConstants.h"
#include "../../utilities/Utils.h"
#include "DMService.h"

using namespace std;

#define SERVICE_NAME             L"AzureDeviceManagementClient"
#define SERVICE_DISPLAY_NAME     L"AzureDeviceManagementClient"
#define SERVICE_START_TYPE       SERVICE_DEMAND_START
#define SERVICE_DEPENDENCIES     L""
#define SERVICE_ACCOUNT          L"NT AUTHORITY\\SYSTEM"
#define SERVICE_PASSWORD         L""

using namespace DMCommon;
using namespace DMUtils;
using namespace Microsoft::Azure::DeviceManagement::Client;

int wmain(int argc, wchar_t *argv[])
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    shared_ptr<ServiceParameters> serviceParameters = make_shared<ServiceParameters>();
    serviceParameters->Load();
    TRACELINE(LoggingLevel::Verbose, L"Service parameters loaded!");

    // Configure logging...
    wstring wideLogFileName = MultibyteToWide(serviceParameters->GetLogFilePath().c_str());
    wstring wideLogFileNamePrefix = MultibyteToWide(LogFilePrefix).c_str();
    gLogger.SetLogFilePath(wideLogFileName.c_str(), wideLogFileNamePrefix.c_str());
    gLogger.EnableConsole(true);

    TRACELINE(LoggingLevel::Verbose, L"Logging has been configured!");

    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"install", argv[1] + 1) == 0)
        {
            DMService::Install(
                SERVICE_NAME,               // Name of service
                SERVICE_DISPLAY_NAME,       // Name to display
                SERVICE_START_TYPE,         // Service start type
                SERVICE_DEPENDENCIES,       // Dependencies
                SERVICE_ACCOUNT,            // Service running account
                SERVICE_PASSWORD            // Password of the account
            );
        }
        else if (_wcsicmp(L"uninstall", argv[1] + 1) == 0)
        {
            DMService::Uninstall(SERVICE_NAME);
        }
        else if (_wcsicmp(L"debug", argv[1] + 1) == 0)
        {
            DMService service(SERVICE_NAME, serviceParameters);
            service.RunClient();
        }
        else
        {
            TRACELINE(LoggingLevel::Verbose, L"Parameters:");
            TRACELINE(LoggingLevel::Verbose, L" -install  to install the service.");
            TRACELINE(LoggingLevel::Verbose, L" -uninstall to remove the service.");
            TRACELINE(LoggingLevel::Verbose, L" -debug to run in console (not as a service).");
            TRACELINE(LoggingLevel::Verbose, L"");
        }
    }
    else
    {
        gLogger.EnableConsole(false);
        TRACELINE(LoggingLevel::Verbose, L"Running service...");

        DMService service(SERVICE_NAME, serviceParameters);
        service.RunService();
    }

    TRACELINE(LoggingLevel::Verbose, L"Exiting wmain.");
    return 0;
}

