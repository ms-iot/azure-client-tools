// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "UwpGetInstalledAppsHandler.h"

#define CSPAppManagementPath "./Device/Vendor/MSFT/EnterpriseModernAppManagement/AppManagement/"
#define CSPStoreTypeAppStore "AppStore"
#define CSPStoreTypeNonStore "NonStore"
#define CSPAppName L"Name"
#define CSPAppVersion L"Version"
#define CSPAppInstallDate L"InstallDate"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Management::Deployment;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace UwpAppManagementPlugin {

UwpGetInstalledAppsHandler::UwpGetInstalledAppsHandler() :
    MdmHandlerBase(UwpGetInstalledAppsHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
{
}

void UwpGetInstalledAppsHandler::Start(
    const Json::Value& config,
    bool& active)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    Json::Value logFilesPath = config[JsonTextLogFilesPath];
    if (!logFilesPath.isNull() && logFilesPath.isString())
    {
        wstring wideLogFileName = MultibyteToWide(logFilesPath.asString().c_str());
        wstring wideLogFileNamePrefix = MultibyteToWide(GetId().c_str());
        gLogger.SetLogFilePath(wideLogFileName.c_str(), wideLogFileNamePrefix.c_str());
        gLogger.EnableConsole(true);

        TRACELINE(LoggingLevel::Verbose, "Logging configured.");
    }

    active = true;
}

void UwpGetInstalledAppsHandler::OnConnectionStatusChanged(
    DMCommon::ConnectionStatus status)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    if (status == ConnectionStatus::eOffline)
    {
        TRACELINE(LoggingLevel::Verbose, "Connection Status: Offline.");
    }
    else
    {
        TRACELINE(LoggingLevel::Verbose, "Connection Status: Online.");
    }
}

InvokeResult UwpGetInstalledAppsHandler::Invoke(
    const Json::Value& jsonParameters) noexcept
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
    InvokeResult invokeResult(InvokeContext::eDirectMethod, JsonDirectMethodSuccessCode, JsonDirectMethodEmptyPayload);

    Json::Value reportedObject(Json::objectValue);
    shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

    Operation::RunOperation(GetId(), errorList,
        [&]()
    {
        // Processing Meta Data
        _metaData->FromJsonParentObject(jsonParameters);

        bool appStore = false;
        bool nonStore = false;

        OperationModelT<bool> appSourceStore = Operation::TryGetOptionalSinglePropertyOpBoolParameter(jsonParameters, JsonAppSourceStore);
        if (appSourceStore.present)
        {
            appStore = appSourceStore.value;
        }

        OperationModelT<bool> appSourceNonStore = Operation::TryGetOptionalSinglePropertyOpBoolParameter(jsonParameters, JsonAppSourceNonStore);
        if (appSourceNonStore.present)
        {
            nonStore = appSourceNonStore.value;
        }

        BuildReported(reportedObject, appStore, nonStore, errorList);
    });

    FinalizeAndReport(reportedObject, errorList);

    // Pack return payload
    if (errorList->Count() != 0)
    {
        invokeResult.code = JsonDirectMethodFailureCode;
        invokeResult.payload = errorList->ToJsonObject()[GetId()].toStyledString();
    }
    else
    {
        invokeResult.code = JsonDirectMethodSuccessCode;
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        invokeResult.payload = Json::writeString(builder, reportedObject);
    }

    return invokeResult;
}

void UwpGetInstalledAppsHandler::GetInstalledApps(const std::string store, ApplicationsMap &installedAppsList, shared_ptr<DMCommon::ReportedErrorList> errorList)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    std::function<void(std::vector<std::wstring>&, std::wstring&)> valueHandler =
        [&installedAppsList](vector<wstring>& wideUriTokens, wstring& wideValue)
    {
        // 0/__1___/__2___/__3_/______________4______________/______5______/___6____/_________7_______/_______8_______/______9_____
        // ./Device/Vendor/MSFT/EnterpriseModernAppManagement/AppManagement/AppStore/PackageFamilyName/PackageFullName/PropertyName

        if (wideUriTokens.size() == 10)
        {
            string appPackageFamilyName = WideToMultibyte(wideUriTokens[7].c_str());
            string appStore = WideToMultibyte(wideUriTokens[6].c_str());
            auto appEntry = installedAppsList.find(appPackageFamilyName);
            // If app is not present in the map, create a new entry
            if (appEntry == installedAppsList.end())
            {
                shared_ptr<ApplicationInfo> appInfo = make_shared<ApplicationInfo>(appPackageFamilyName, appStore);
                installedAppsList.insert(ApplicationsMap::value_type(appPackageFamilyName, appInfo));
            }
            else
            {
                // add values 
                if (wideUriTokens[9].compare(CSPAppName) == 0)
                {
                    appEntry->second->name = WideToMultibyte(wideValue.c_str());
                }
                if (wideUriTokens[9].compare(CSPAppVersion) == 0)
                {
                    appEntry->second->version = WideToMultibyte(wideValue.c_str());
                }
                else if (wideUriTokens[9].compare(CSPAppInstallDate) == 0)
                {
                    appEntry->second->installDate = WideToMultibyte(wideValue.c_str());
                }
            }
        }
    };
    Operation::RunOperation(InstalledAppsInfo, errorList,
        [&]()
    {
        string path = CSPAppManagementPath + store + "?list=StructData";
        _mdmProxy.RunGetStructData(path, valueHandler);
    });
}


void UwpGetInstalledAppsHandler::BuildReported(
    Json::Value& reportedObject,
    bool store, 
    bool nonStore,
    std::shared_ptr<DMCommon::ReportedErrorList> errorList)
{
    ApplicationsMap installedApps;
    if (store)
    {
        GetInstalledApps(CSPStoreTypeAppStore, installedApps, errorList);
    }
    if (nonStore)
    {
        GetInstalledApps(CSPStoreTypeNonStore, installedApps, errorList);
    }

    for (auto app : installedApps)
    {
        Json::Value appInfo;
        appInfo[JsonPkgFamilyName] = Json::Value(app.second->packageFamilyName);
        appInfo[JsonVersion] = Json::Value(app.second->version);
        appInfo[JsonInstallDate] = Json::Value(app.second->installDate);
        replace(app.second->packageFamilyName.begin(), app.second->packageFamilyName.end(), '.', '_');
        reportedObject[app.second->packageFamilyName] = Json::Value(appInfo);
    }
}
}}}}
