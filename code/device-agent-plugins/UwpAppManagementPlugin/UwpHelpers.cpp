// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "UwpHelpers.h"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

constexpr wchar_t IotStartupExe[] = L"%windir%\\system32\\iotstartup.exe";
constexpr wchar_t AddCmd[] = L" add ";
constexpr wchar_t RemoveCmd[] = L" remove ";
constexpr wchar_t StartCmd[] = L" run ";
constexpr wchar_t StopCmd[] = L" stop ";
constexpr wchar_t Headless[] = L" headless ";
constexpr wchar_t Headed[] = L" headed ";
constexpr wchar_t Startup[] = L" startup ";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace UwpAppManagementPlugin {

    const std::wstring UwpHelpers::IotStartupExePath()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        wchar_t expanded[MAX_PATH] = { 0 };
        size_t length = ExpandEnvironmentStrings(IotStartupExe, expanded, MAX_PATH);
        if (length == 0)
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to expand environment strings");
        }
        if (length > MAX_PATH)
        {
            throw DMException(DMSubsystem::Windows, ERROR_INSUFFICIENT_BUFFER, "Expanded path string longer than MAX_PATH");
        }

        return wstring(expanded);
    }

    void UwpHelpers::StartStopApp(const string& appId, bool start)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        unsigned long returnCode;
        string output;
        wstring cmd = IotStartupExePath();

        cmd = start ? cmd + StartCmd : cmd + StopCmd;
        cmd += MultibyteToWide(appId.c_str());
        DMUtils::Process::Launch(cmd, returnCode, output);
        if (returnCode != 0)
        {
            throw DMException(DMSubsystem::IotStartup, returnCode, "Error: iotstartup.exe returned an error code.");
        }
    }

    void UwpHelpers::UpdateAppStartup(const std::string& pkgFamilyName, StartupType startupType)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        wstring cmd = IotStartupExePath();

        switch (startupType)
        {
            case StartupType::eForeground:
                cmd += AddCmd;
                cmd += Headed;
                break;
            case StartupType::eBackground:
                cmd += AddCmd;
                cmd += Headless;
                break;
            case StartupType::eNone:
                // In case of none, we assume its a headless app. Removing headed app is not supported. 
                cmd += RemoveCmd;
                cmd += Headless;
                break;
            default:
                return;
        }

        cmd += MultibyteToWide(pkgFamilyName.c_str());

        unsigned long returnCode = 0;
        string output;
        DMUtils::Process::Launch(cmd, returnCode, output);
        if (returnCode != 0)
        {
            if ((startupType == StartupType::eNone) && ((HRESULT)returnCode == E_INVALIDARG))
            {
                // OK to return if the app was not registered as startup and we are trying to remove it
                return;
            }
            throw DMException(DMSubsystem::IotStartup, returnCode, "Error: iotstartup.exe returned an error code.");
        }
    }

    std::string UwpHelpers::GetStartupApp()
    {
        wstring cmd = IotStartupExePath();
        cmd += Startup;
        cmd += Headed;

        unsigned long returnCode = 0;
        string output;
        DMUtils::Process::Launch(cmd, returnCode, output);
        if (returnCode != 0)
        {
            if ((HRESULT)returnCode == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                // In case of IoT Enterprise just return empty string
                return "";
            }
            else
            {
                throw DMException(DMSubsystem::IotStartup, returnCode, "Error: iotstartup.exe returned an error code.");
            }
        }

        // IotStartup returns startup app in following format 
        // Headed   : 8112d70e-a549-4378-96a9-63e8491e3d66_7ywy5sjsre78e!App
        // Extract the pkgFamilyName betwee : and !

        std::size_t startPos = output.find(':') + 2; 
        std::size_t endPos = output.find('!');
        std::string pkgFamilyName = output.substr(startPos, endPos - startPos);
        return pkgFamilyName;
    }
}}}}