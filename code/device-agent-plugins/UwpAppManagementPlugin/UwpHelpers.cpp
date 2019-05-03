// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "UwpHelpers.h"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

constexpr wchar_t IotStartupExe[] = L"%windir%\\system32\\iotstartup.exe";
constexpr wchar_t ListCmd[] = L" list ";
constexpr wchar_t AddCmd[] = L" add ";
constexpr wchar_t RemoveCmd[] = L" remove ";
constexpr wchar_t StartCmd[] = L" run ";
constexpr wchar_t StopCmd[] = L" stop ";
constexpr wchar_t Headless[] = L" headless ";
constexpr wchar_t Headed[] = L" headed ";
constexpr wchar_t Startup[] = L" startup ";

constexpr char HeadedPrefix[] = "Headed   :";
constexpr char HeadlessPrefix[] = "Headless :";

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

    StartupType UwpHelpers::GetAppStartupType(
        const std::string& pkgFamilyName)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINEP(LoggingLevel::Verbose, "Getting app startup for: ", pkgFamilyName.c_str());

        wstring cmd = IotStartupExePath();
        cmd += ListCmd;
        cmd += MultibyteToWide(pkgFamilyName.c_str());

        unsigned long returnCode = 0;
        string output;
        DMUtils::Process::Launch(cmd, returnCode, output);
        if (returnCode != 0)
        {
            throw DMException(DMSubsystem::IotStartup, returnCode, "Error: iotstartup.exe returned an error code.");
        }

        StartupType startupType = StartupType::eUndefined;

        if (output.size() > 0)
        {
            if (0 == strncmp(HeadedPrefix, output.c_str(), strlen(HeadedPrefix)))
            {
                TRACELINE(LoggingLevel::Verbose, "Foreground");
                startupType = StartupType::eForeground;
            }
            else if (0 == strncmp(HeadlessPrefix, output.c_str(), strlen(HeadedPrefix)))
            {
                TRACELINE(LoggingLevel::Verbose, "Background");
                startupType = StartupType::eBackground;
            }
        }

        return startupType;
    }

    void UwpHelpers::UpdateAppStartup(const std::string& pkgFamilyName, StartupType startupType)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINEP(LoggingLevel::Verbose, "Updating app startup for: ", pkgFamilyName.c_str());

        wstring cmd = IotStartupExePath();

        if (GetAppStartupType(pkgFamilyName) == startupType)
        {
            TRACELINE(LoggingLevel::Verbose, "Start-up type is already in sync. Returning...");
            return;
        }

        switch (startupType)
        {
            case StartupType::eForeground:
                TRACELINE(LoggingLevel::Verbose, "Adding to the headed list.");
                cmd += AddCmd;
                cmd += Headed;
                break;
            case StartupType::eBackground:
                TRACELINE(LoggingLevel::Verbose, "Adding to the headless list.");
                cmd += AddCmd;
                cmd += Headless;
                break;
            case StartupType::eNone:
                TRACELINE(LoggingLevel::Verbose, "Removing from the headless list.");
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