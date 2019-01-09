// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    static const unsigned int ErrorEmbeddedModeDisabled = 1;

    static const char* ServiceConfigurationFile = "AzureDeviceManagementClient.json";

    static const char* LogFilePrefix = "Core";
    static const char* LogFilePathDefaultValue = "";
    static const char* PluginManifestPathDefaultValue = "";
    static const unsigned int SasExpiryIntervalDefaultValue = 24 * 3600; // in seconds
    static const unsigned int SasRenewalGracePeriodDefaultValue = 1 * 3600; // in seconds
    static const char* DpsUriDefaultValue = "";
    static const char* DpsScopeIdDefaultValue = "";
    static const unsigned int ConnectionStringExpiryDefaultValue = 1 * 3600;    // in seconds
    static const unsigned int ConnectionStringSlotNumberDefaultValue = 0;
    static const char* DefaultDebugConnectionString = "";
    static const char* DefaultModuleId = "WindowsIoTDM";

    static const wchar_t* ServiceName = L"AzureDeviceManagementClient";
    static const wchar_t* RootRegistryKey = L"SYSTEM\\CurrentControlSet\\Services\\AzureDeviceManagementClient\\Parameters";

    static const int DirectMethodSuccess = 200;
    static const int MessageLoopWait = 10;


    static const char* PluginManifestFolder = ".";

}}}}
