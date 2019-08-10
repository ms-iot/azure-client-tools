// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <wrl\client.h>
#include "..\Utilities\TpmSupport.h"
#include "..\Utilities\Logger.h"
#include "..\Utilities\Registry.h"
#include "..\Utilities\DMString.h"
#include "..\Utilities\AutoCloseTypes.h"
#include "..\Utilities\ResultMacros.h"

#include "NetConnection.h"

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/xlogging.h"

#include "azure_c_shared_utility/connection_string_parser.h"
#include "azure_macro_utils/macro_utils.h"

#include "azure_prov_client/prov_device_ll_client.h"
#include "azure_prov_client/prov_security_factory.h"
#include "azure_prov_client/prov_transport_http_client.h"

#include "iothub_service_client_auth.h"
#include "iothub_registrymanager.h"

#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/azure_base64.h"

#define IOTDPSCLIENT_PARAMETERS_REGPATH             L"SYSTEM\\CurrentControlSet\\Services\\AzureDeviceManagementClient\\Parameters"
#define IOTDPSCLIENT_PARAMETERS_REGNAME_DPSSCOPE    L"IdScope"
#define IOTDPSCLIENT_PARAMETERS_REGNAME_TPMSLOT     L"logicalDeviceId"
#define IOTDPSCLIENT_PARAMETERS_REGNAME_MODULES     L"modules"

using namespace std;
using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace Microsoft::WRL;

extern void xlogging_set_log_function(LOGGER_LOG log_function);
void LoggingForDpsSdk(LOG_CATEGORY log_category, const char* file, const char* func, const int line, unsigned int /*options*/, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    string fmt("");
    switch (log_category)
    {
    case AZ_LOG_INFO:
        fmt = "Info: ";
        break;
    case AZ_LOG_ERROR:
    {
        int size = _scprintf("Error: File:%s Func:%s Line:%d ", file, func, line);
        vector<char> message(size + 1, '\0');
        sprintf_s(message.data(), message.size(), "Error: File:%s Func:%s Line:%d ", file, func, line);
        fmt = message.data();
    }
    break;
    default:
        break;
    }
    fmt += format;

    int size = _vscprintf(fmt.data(), args);
    vector<char> message(size + 1, '\0');
    vsprintf_s(message.data(), message.size(), fmt.data(), args);
    TRACE(LoggingLevel::Verbose, message.data());

    va_end(args);
}


#define DPS_SUCCESS 1
#define DPS_FAILURE -1
#define DPS_RUNNING 0

typedef struct CLIENT_SAMPLE_INFO_TAG
{
    unsigned int sleep_time;
    char* iothub_uri;
    char* access_key_name;
    char* device_key;
    char* device_id;
    int registration_complete;
    int slot;
} CLIENT_SAMPLE_INFO;

CLIENT_SAMPLE_INFO dps_info;

MU_DEFINE_ENUM_STRINGS(PROV_DEVICE_RESULT, PROV_DEVICE_RESULT_VALUE);
MU_DEFINE_ENUM_STRINGS(PROV_DEVICE_REG_STATUS, PROV_DEVICE_REG_STATUS_VALUES);


static void registation_status_callback(PROV_DEVICE_REG_STATUS reg_status, void* user_context)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);

    if (user_context == NULL)
    {
        TRACE(LoggingLevel::Verbose, "user_context is NULL");
    }
    else
    {
        CLIENT_SAMPLE_INFO* local_dps_info = (CLIENT_SAMPLE_INFO*)user_context;

        TRACEP(LoggingLevel::Verbose, "Provisioning Status: ", MU_ENUM_TO_STRING(PROV_DEVICE_REG_STATUS, reg_status));
        if (reg_status == PROV_DEVICE_REG_STATUS_CONNECTED)
        {
            local_dps_info->sleep_time = 600;
        }
        else if (reg_status == PROV_DEVICE_REG_STATUS_REGISTERING)
        {
            local_dps_info->sleep_time = 900;
        }
        else if (reg_status == PROV_DEVICE_REG_STATUS_ASSIGNING)
        {
            local_dps_info->sleep_time = 1200;
        }
    }
}

static HRESULT store_module_info_in_tpm(IOTHUB_MODULE *moduleInfo, const string& hostName, int slot)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);

    wstring fullUri(LIMPET_STRING_SIZE, L'\0');
    HRESULT hr = S_OK;

    string primaryKey = moduleInfo->primaryKey;
    if (TpmSupport::StoreHmacKey(slot, primaryKey) != TPM_RC_SUCCESS)
    {
        TRACEP(LoggingLevel::Error, "TpmSupport::StoreHmacKey failed", "");
        hr = E_FAIL;
    }
    else if (StringCchPrintfW(&fullUri[0], fullUri.capacity(), L"%S/%S/%S", hostName.c_str(), moduleInfo->deviceId, moduleInfo->moduleId) != ERROR_SUCCESS)
    {
        TRACEP(LoggingLevel::Error, "StringCchPrintfW failed", "");
        hr = E_FAIL;
    }
    else if (TpmSupport::StoreIothubUri(slot, fullUri, wstring(L"")) != TPM_RC_SUCCESS)
    {
        TRACEP(LoggingLevel::Error, "TpmSupport::StoreIothubUri failed", "");
        hr = E_FAIL;
    }

    return hr;
}

static HRESULT create_module(int slot, const string& hostUri, const string& deviceId, const string& moduleName, const string& deviceConnectionString)
{
    // Has the module connection info already been provisioned?
    bool provisioned = false;
    if (TpmSupport::IsDeviceProvisioned(slot, &provisioned) == TPM_RC_SUCCESS && provisioned)
    {
        // Module connection info is present in TPM
        TRACEP(LoggingLevel::Verbose, L"Module already provisioned in TPM: ", moduleName.c_str());
        return S_OK;
    }

    TRACEP(LoggingLevel::Verbose, "Create module: ", moduleName.c_str());

    IOTHUB_REGISTRY_MODULE_CREATE moduleCreate;
    IOTHUB_MODULE moduleInfo;
    HRESULT hr = S_OK;

    // We set the initial auth type to none, to simulate and test more closely how Edge
    // modules are created.  We'll upgrade this to SPK after creation.
    moduleCreate.version = IOTHUB_REGISTRY_MODULE_CREATE_VERSION_1;
    moduleCreate.authMethod = IOTHUB_REGISTRYMANAGER_AUTH_SPK;
    moduleCreate.deviceId = deviceId.c_str();
    moduleCreate.moduleId = moduleName.c_str();
    moduleCreate.primaryKey = "";
    moduleCreate.secondaryKey = "";
    moduleCreate.managedBy = NULL;

    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE service_auth_from_device_connection = NULL;
    IOTHUB_REGISTRYMANAGER_RESULT iothub_registrymanager_result;
    IOTHUB_REGISTRYMANAGER_HANDLE iothub_registrymanager_handle = NULL;

    memset(&moduleInfo, 0, sizeof(moduleInfo));
    moduleInfo.version = IOTHUB_MODULE_VERSION_1;

    if ((service_auth_from_device_connection = IoTHubServiceClientAuth_CreateFromSharedAccessSignature(deviceConnectionString.c_str())) == NULL)
    {
        TRACEP(LoggingLevel::Error, "IoTHubServiceClientAuth_CreateFromConnectionString(%s) failed", deviceConnectionString.c_str());
        hr = E_FAIL;
    }
    else if ((iothub_registrymanager_handle = IoTHubRegistryManager_Create(service_auth_from_device_connection)) == NULL)
    {
        TRACEP(LoggingLevel::Error, "IoTHubServiceClientAuth_CreateFromConnectionString(%s) failed", deviceConnectionString.c_str());
        hr = E_FAIL;
    }
    // Try getting an existing module first, if that fails, try creating one
    else if (((iothub_registrymanager_result = IoTHubRegistryManager_GetModule(iothub_registrymanager_handle, deviceId.c_str(), moduleName.c_str(), &moduleInfo)) != IOTHUB_REGISTRYMANAGER_OK) &&
             ((iothub_registrymanager_result = IoTHubRegistryManager_CreateModule(iothub_registrymanager_handle, &moduleCreate, &moduleInfo)) != IOTHUB_REGISTRYMANAGER_OK))
    {
        TRACEP(LoggingLevel::Error, "IoTHubRegistryManager_CreateModule failed, err=%d", iothub_registrymanager_result);
        hr = E_FAIL;
    }
    // Ensure that the module info returned corresponds to the desired module
    else if (strcmp(moduleInfo.moduleId, moduleName.c_str()) != 0)
    {
        TRACEP(LoggingLevel::Error, "Module name expected:", moduleName.c_str());
        TRACEP(LoggingLevel::Error, "Returned module name from IoTHubRegistryManager_CreateModule (%s)", moduleInfo.moduleId);
        hr = E_FAIL;
    }
    // Ensure that the module info returned corresponds to the desired device
    else if (strcmp(deviceId.c_str(), moduleInfo.deviceId) != 0)
    {
        TRACEP(LoggingLevel::Error, "Device ID expected:", deviceId.c_str());
        TRACEP(LoggingLevel::Error, "Returned device id from IoTHubRegistryManager_CreateModule (%s)", moduleInfo.deviceId);
        hr = E_FAIL;
    }
    else
    {
        hr = store_module_info_in_tpm(&moduleInfo, hostUri, slot);
    }

    IoTHubRegistryManager_FreeModuleMembers(&moduleInfo);

    if (iothub_registrymanager_handle != NULL)
    {
        IoTHubRegistryManager_Destroy(iothub_registrymanager_handle);
    }
    if (service_auth_from_device_connection != NULL)
    {
        IoTHubServiceClientAuth_Destroy(service_auth_from_device_connection);
    }

    return hr;
}

static HRESULT create_modules(int deviceSlot, const map<string, UINT32>& modules)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);

    string uriData(LIMPET_STRING_SIZE, '\0');
    size_t device_id_separator = (size_t)-1;
    DWORD validity = 3600;
    HRESULT hr = S_OK;

    wstring wideDeviceConnectionString(LIMPET_STRING_SIZE, L'\0');
    size_t cchOutput = LIMPET_STRING_SIZE;

    // Get Device URI (hostUri/deviceId)
    if ((hr = TpmSupport::GetIothubUri(deviceSlot, uriData)) != TPM_RC_SUCCESS)
    {
        TRACEP(LoggingLevel::Error, "TpmSupport::GetIothubUri failed, err=%d", hr);
        hr = E_FAIL;
    }
    else if ((device_id_separator = uriData.find('/')) == string::npos)
    {
        // The device URI does not conform to expected pattern: hostUri/deviceId
        TRACEP(LoggingLevel::Error, "Device URI does not conform to expected pattern (hostUri/deviceId): %s", uriData.c_str());
        hr = E_FAIL;
    }
    else if (uriData.find('/', device_id_separator + 1) != string::npos)
    {
        // The device URI does not conform to expected pattern, may be module: hostUri/deviceId/xxx
        TRACEP(LoggingLevel::Error, "Device URI does not conform to expected pattern (may be a module?): %s", uriData.c_str());
        hr = E_FAIL;
    }
    else if ((hr = TpmSupport::GetAzureConnectionString(deviceSlot, validity, wideDeviceConnectionString)) != TPM_RC_SUCCESS)
    {
        TRACEP(LoggingLevel::Error, "TpmSupport::GetAzureConnectionString failed, err=%d", hr);
        hr = E_FAIL;
    }
    else
    {
        auto deviceConnectionString = WideToMultibyte(wideDeviceConnectionString.c_str());
        auto deviceId = uriData.substr(device_id_separator + 1);
        uriData[device_id_separator] = '\0';

        // Iterate over modules and create or store connection info for each as needed
        for (const auto& moduleInfo : modules)
        {
            const auto& moduleSlot = moduleInfo.second;
            const auto& module = moduleInfo.first;
            TRACEP(LoggingLevel::Verbose, L"Module to create: ", module.c_str());
            TRACEP(LoggingLevel::Verbose, L"Module slot: ", moduleSlot);
            hr = create_module(moduleSlot, uriData, deviceId, module, deviceConnectionString);
            if (FAILED(hr))
            {
                return hr;
            }
        }
    }

    return hr;
}

static void register_device_callback(PROV_DEVICE_RESULT register_result, const char* iothub_uri, const char* device_id, void* user_context)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);

    if (user_context == NULL)
    {
        printf("user_context is NULL\r\n");
    }
    else
    {
        CLIENT_SAMPLE_INFO* reg_dps_info = (CLIENT_SAMPLE_INFO*)user_context;
        if (register_result == PROV_DEVICE_RESULT_OK)
        {
            string fullUri = iothub_uri;
            fullUri += "/";
            fullUri += device_id;

            // Store connection string in TPM:
            //   limpet <slot> -SUR <uri>/<deviceId>
            TRACEP(LoggingLevel::Verbose, "call limpet <slot> -SUR: ", fullUri.c_str());
            wstring wfullUri{ MultibyteToWide(fullUri.c_str()) };
            wstring wName = L"";
            TpmSupport::StoreIothubUri(reg_dps_info->slot, wfullUri, wName);
            reg_dps_info->registration_complete = DPS_SUCCESS;
        }
        else
        {
            TRACEP(LoggingLevel::Verbose, "Failure encountered on registration: %s\r\n", MU_ENUM_TO_STRING(PROV_DEVICE_RESULT, register_result));
            reg_dps_info->registration_complete = DPS_FAILURE;
        }
    }
}

HRESULT DpsRegisterDevice(
    UINT32 tpmDeviceSlotNumber,
    const string dpsUri,
    const string dpsScopeId
)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);
    TRACEP(LoggingLevel::Verbose, L"tpm slot number: ", tpmDeviceSlotNumber);
    TRACEP(LoggingLevel::Verbose, "dpsUri: ", dpsUri.data());
    TRACEP(LoggingLevel::Verbose, "id scope: ", dpsScopeId.c_str());

    xlogging_set_log_function(&LoggingForDpsSdk);
    HRESULT hr = S_OK;
    bool provisioned = false;
    hr = IsDeviceProvisionedInAzure(tpmDeviceSlotNumber, &provisioned);

    if((FAILED(hr)) || (!provisioned))
    {
        memset(&dps_info, 0, sizeof(CLIENT_SAMPLE_INFO));
        dps_info.slot = tpmDeviceSlotNumber;

        TpmSupport::EmptyTpmSlot(tpmDeviceSlotNumber);

        do
        {
            TRACE(LoggingLevel::Verbose, "Start registration process");
            dps_info.registration_complete = DPS_RUNNING;
            dps_info.sleep_time = 10;

            AutoCloseCom com;
            RETURN_HR_IF_FAILED(com.GetInitializationStatus());

            ComPtr<CNetConnectionState> net;
            hr = MakeAndInitialize<CNetConnectionState>(&net);
            FAIL_FAST_IF_FAILED(hr, L"error occurred in initializing  Network List Manager interfaces.");
            RETURN_HR_IF_FAILED(net->WaitForConnection());

            SECURE_DEVICE_TYPE hsm_type = SECURE_DEVICE_TYPE_TPM;

            hr = platform_init();
            if (FAILED(hr))
            {
                TRACE(LoggingLevel::Error, "Failed calling platform_init");
                return hr;
            }

            hr = prov_dev_security_init(hsm_type);
            if (FAILED(hr))
            {
                TRACE(LoggingLevel::Error, "Failed calling prov_dev_security_init");
                return hr;
            }

            PROV_DEVICE_LL_HANDLE handle;
            if ((handle = Prov_Device_LL_Create(dpsUri.data(), dpsScopeId.data(), Prov_Device_HTTP_Protocol)) == NULL)
            {
                TRACE(LoggingLevel::Error, "failed calling DPS_LL_Create");
                hr = E_FAIL;
                return hr;
            }

            if (Prov_Device_LL_Register_Device(handle, register_device_callback, &dps_info, registation_status_callback, &dps_info) != PROV_DEVICE_RESULT_OK)
            {
                TRACE(LoggingLevel::Error, "failed calling Prov_Device_LL_Register_Device");
                hr = E_FAIL;
                return hr;
            }

            do
            {
                Prov_Device_LL_DoWork(handle);
                ThreadAPI_Sleep(dps_info.sleep_time);
            } while (DPS_RUNNING == dps_info.registration_complete);

            Prov_Device_LL_Destroy(handle);

            if (DPS_SUCCESS == dps_info.registration_complete)
            {
                break;
            }
            else
            {
                ThreadAPI_Sleep(5000);
            }

            TRACE(LoggingLevel::Error, "Registration failed, retry");

        } while (true);
    }

    TRACE(LoggingLevel::Verbose, "Exiting DoDpsWork");

    return hr;
}

HRESULT DpsRegisterDeviceAndModules(
    UINT32 tpmDeviceSlotNumber,
    const string dpsUri,
    const string dpsScopeId,
    UINT32 tpmDmModuleSlotNumber,
    const string dmModuleId,
    const map<string, UINT32> otherModules
)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);

    HRESULT hr = DpsRegisterDevice(tpmDeviceSlotNumber, dpsUri, dpsScopeId);
    if (FAILED(hr))
    {
        TRACE(LoggingLevel::Error, "failed calling DpsRegisterDevice");
        return hr;
    }

    TRACE(LoggingLevel::Verbose, "Register modules as needed");
    map<string, UINT32> modulesMap;
    for (const auto& mod : otherModules) {
        modulesMap[mod.first] = mod.second;
    }
    modulesMap[dmModuleId] = tpmDmModuleSlotNumber;
    return create_modules(tpmDeviceSlotNumber, modulesMap);
}

HRESULT DpsRegisterDeviceWithoutModules(
    UINT32 tpmDeviceSlotNumber,
    const wstring wdpsUri,
    const wstring wdpsScopeId
)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);

    string dpsScopeId = WideToMultibyte(wdpsScopeId.c_str());
    string dpsUri = WideToMultibyte(wdpsUri.c_str());

    return DpsRegisterDevice(tpmDeviceSlotNumber, dpsUri, dpsScopeId);
}

