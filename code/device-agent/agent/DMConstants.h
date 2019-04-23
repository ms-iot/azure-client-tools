// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    static const unsigned int ErrorEmbeddedModeDisabled = 1;

    static const char* ServiceConfigurationFile = "AzureDeviceManagementClient.json";

    static const char* LogFilePrefix = "Core";
    static const char* LogFilePathDefaultValue = "";
    static const char* PluginManifestPathDefaultValue = "";
    static const char* PluginsDataPathDefaultValue = "..\\DMData";
    static const unsigned int SasExpiryIntervalDefaultValue = 24 * 3600; // in seconds
    static const unsigned int SasRenewalGracePeriodDefaultValue = 1 * 3600; // in seconds
    static const bool DpsIsEnabledDefaultValue = true;
    static const char* DpsUriDefaultValue = "";
    static const char* DpsScopeIdDefaultValue = "";
    static const unsigned int ConnectionStringExpiryDefaultValue = 1 * 3600;    // in seconds
    static const unsigned int DeviceSlotNumberDefaultValue = 0;
    static const unsigned int DmModuleSlotNumberDefaultValue = 0;
    static const char* DefaultConnectionString = "";
    static const char* DefaultModuleId = "WindowsIoTDM";
    static const char* DefaultAzureInterfaceType = "raw";

    static const wchar_t* ServiceName = L"AzureDeviceManagementClient";
    static const wchar_t* RootRegistryKey = L"SYSTEM\\CurrentControlSet\\Services\\AzureDeviceManagementClient\\Parameters";
    static const wchar_t* WDPRegisteryScopeId = L"IdScope";
    static const wchar_t* WDPRegisteryLogicalDeviceId = L"LogicalDeviceId";

    static const int DirectMethodSuccess = 200;
    static const int MessageLoopWait = 10;

    static const char* PluginManifestFolder = ".";

    // Manifest
    static const long AGENT_ERR_PLUGIN_MANIFEST_INVALID_SCHEMA = -1;
    static const long AGENT_ERR_PLUGIN_MANIFEST_MISSING_CODE_FILENAME = -2;
    static const long AGENT_ERR_PLUGIN_MANIFEST_MISSING_OR_INVALID_DIRECT_ATTRIBUTE = -3;
    static const long AGENT_ERR_PLUGIN_MANIFEST_MISSING_OR_INVALID_OUTOFPROC_ATTRIBUTE = -4;
    static const long AGENT_ERR_PLUGIN_MANIFEST_MISSING_OR_INVALID_KEEPALIVE_ATTRIBUTE = -5;
    static const long AGENT_ERR_PLUGIN_MANIFEST_INVALID_KEEPALIVE_VALUE = -6;

}}}}
