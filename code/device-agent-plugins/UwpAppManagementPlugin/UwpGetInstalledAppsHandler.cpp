// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "UwpGetInstalledAppsHandler.h"

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


        GetInstalledApps(reportedObject, errorList);
    });

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

void UwpGetInstalledAppsHandler::GetInstalledApps(
    Json::Value& reportedObject,
    std::shared_ptr<DMCommon::ReportedErrorList> errorList)
{
     TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

     PackageManager^ packageManager = ref new PackageManager;
     IIterable<Package^>^ packages = packageManager->FindPackages();
     IIterator<Package^>^ it = packages->First();
     Json::Value appList;
     while (it->HasCurrent)
     {
         Package^ package = it->Current;
         std::string pkgFamilyName = Utils::WideToMultibyte(package->Id->FamilyName->Data()).c_str();
         Json::Value appInfo;
         
         appInfo[JsonPkgFamilyName] = Json::Value(pkgFamilyName);
         appInfo[JsonVersion] = Json::Value(PackageVersion(package));
         appList.append(appInfo);
         it->MoveNext();
     }

     reportedObject[JsonInstalledApps] = Json::Value(appList);
}
}}}}
