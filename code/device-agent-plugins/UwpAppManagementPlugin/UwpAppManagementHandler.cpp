// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <collection.h>
#include <was/storage_account.h>
#include <was/blob.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>
#include "PluginJsonConstants.h"
#include "UwpAppManagementHandler.h"
#include "UwpHelpers.h"
#include "AppDesiredState.h"

const char FileConfigurationSeparator = '\\';
const char DepsSourcesSepatator = ';';

using namespace DMCommon;
using namespace DMUtils;
using namespace std;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Management::Deployment;
using namespace Windows::Globalization::DateTimeFormatting;

constexpr char InterfaceVersion[] = "1.0.0";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace UwpAppManagementPlugin {

    class InitializeWinRT
    {
    private:
        bool roInitialized;
    public:
        InitializeWinRT()
        {
            roInitialized = false;
            if (SUCCEEDED(::RoInitialize(RO_INIT_MULTITHREADED)))
            {
                roInitialized = true;
            }
        }

        ~InitializeWinRT()
        {
            if (roInitialized)
            {
                RoUninitialize();
            }
        }
    };

    UwpAppManagementHandler::UwpAppManagementHandler() :
        MdmHandlerBase(UwpAppManagementHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, InterfaceVersion))
    {
    }

    void UwpAppManagementHandler::Start(
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

    void UwpAppManagementHandler::OnConnectionStatusChanged(
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

    InvokeResult UwpAppManagementHandler::Invoke(
        const Json::Value& desiredConfig) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Initialize Windows runtime
        InitializeWinRT winrt;

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDesiredState);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        Operation::RunOperation(GetId(), errorList,
            [&]()
        {
            // Make sure this is not a transient state
            if (Operation::IsRefreshing(desiredConfig))
            {
                return;
            }

            // Processing Meta Data
            _metaData->FromJsonParentObject(desiredConfig);

            // Report refreshing
            ReportRefreshing();

            string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

            //Compare interface version with the interface version sent by service
            if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
            {
                // Apply new state
                OperationModelT<string> connectionString = Operation::TryGetStringJsonValue(desiredConfig, JsonConnectionString);

                std::vector<std::pair<Package^, StartupType>> installedApps;
                std::vector<std::string> uninstalledApps;

                ModifyApplicationHandler(desiredConfig, connectionString, installedApps, uninstalledApps, errorList);

                // Report current state
                if (_metaData->GetReportingMode() == JsonReportingModeDefault)
                {
                    BuildReported(reportedObject, installedApps, uninstalledApps);
                }
                else
                {
                    EmptyReported(reportedObject);
                }
                _metaData->SetDeviceInterfaceVersion(InterfaceVersion);
            }
            else
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_INTERFACE_VERSION, "Service solution is trying to talk with Interface Version that is not supported.");
            }
        });

        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }

    void UwpAppManagementHandler::ModifyApplicationHandler(
        const Json::Value& desiredConfig,
        const DMCommon::OperationModelT<std::string>& connectionString,
        std::vector<std::pair<Package^, StartupType>> &modifiedApps,
        std::vector<std::string> &uninstalledApps,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        Operation::RunOperation(JsonApps, errorList,
            [&]()
        {
            // Get the desire state for the apps 
            OperationModel uwpAppsJsonObject = Operation::TryGetJsonValue(desiredConfig, JsonApps);
            if (!uwpAppsJsonObject.present)
            {
                return;
            }

            Json::Value reorderedConfig; 
            AppDesiredStateList reorderedDesiredStates; 

            // Reorder and validate the desired config
            ReorderAndValidate(uwpAppsJsonObject.value, reorderedDesiredStates);

            // Loop through all the apps
            for (auto const &appDesiredState : reorderedDesiredStates)
            {
                // For each app 
                Operation::RunOperation(appDesiredState.packageFamilyId, errorList, [&]() {

                    switch (appDesiredState.action)
                    {
                        case AppDesiredAction::eUninstall:
                            UninstallApp(appDesiredState.packageFamilyName);
                            uninstalledApps.push_back(appDesiredState.packageFamilyName);
                            break;

                        case AppDesiredAction::eDowngrade:

                            // For downgrade scenario, uninstall first;
                            UninstallApp(appDesiredState.packageFamilyName);
                            __fallthrough;

                        case AppDesiredAction::eInstall:
                        case AppDesiredAction::eUpgrade:
                        {
                            if (!connectionString.present)
                            {
                                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON, "Connection string for app packages is missing");
                            }

                            string appxPath = DownloadAppPackageFromBlob(connectionString.value, appDesiredState.appxSource, errorList);
                            vector <string> depsPath;
                            if (!appDesiredState.depsSources.empty())
                            {
                                // this is a semicolon delimited string
                                vector<string> depsSourceVector;
                                SplitString(appDesiredState.depsSources, DepsSourcesSepatator, depsSourceVector);
                                for (auto const& dep : depsSourceVector)
                                {
                                    depsPath.push_back(DownloadAppPackageFromBlob(connectionString.value, dep, errorList));
                                }
                            }

                            InstallApp(appDesiredState.package, appDesiredState.packageFamilyName, appxPath, depsPath, appDesiredState.launchAfterInstall);
                            __fallthrough;
                        }
        
                        case AppDesiredAction::eQuery:
                        default:
                        {
                            Package^ installedApp = FindApp(appDesiredState.packageFamilyName);
                            UwpHelpers::UpdateAppStartup(appDesiredState.packageFamilyName, appDesiredState.startupType);
                            
                            // Add to installedApp list for reporting
                            modifiedApps.push_back(pair<Package^, StartupType>(installedApp, appDesiredState.startupType));
                        }
                    }

                });
            }
        });
    }

    void UwpAppManagementHandler::ReorderAndValidate(const Json::Value& desiredConfig, AppDesiredStateList& reorderedConfig)
    {
        // If we are switching the foreground app from App1 to App2, App2 must be processed first.
        // For example:
        // - Startup is set to App1.
        // - Desired state is:
        //   - App1 : none
        //   - App2 : foreground
        // We have to process App2 first and that will implicitly apply App1 : none.

        // Note that reorderedConfig are not only the just-changed ones - but all the desired states for all apps.
        // This covers the case where the user sets a new foreground app and forgets to set the old one to 'none'.

        // Loop through all the apps

        vector<string> foreGroundStartupList;
        vector<string> removeStartupList; 
        vector<string> uninstallList;
        string currentStartupApp = UwpHelpers::GetStartupApp();

        // Parse the JSON and populate the list
        for (Json::Value::const_iterator itr = desiredConfig.begin(); itr != desiredConfig.end(); itr++)
        {
            AppDesiredState desiredState(itr.key().asString());

            desiredState.packageFamilyName = Operation::GetStringJsonValue(*itr, JsonPkgFamilyName); // Required parameter.

            // Get the desired version
            string desiredVersion = Operation::GetStringJsonValue(*itr, JsonVersion);  // Required parameter.
            if (desiredVersion.compare(JsonNotInstalled) == 0)
            {
                desiredState.action = AppDesiredAction::eUninstall;
                uninstallList.push_back(desiredState.packageFamilyName);
                reorderedConfig.push_back(desiredState);
                continue;
            }
            else
            {
                desiredState.version = Version(desiredVersion);
            }

            OperationModelT<string> startup = Operation::TryGetStringJsonValue(*itr, JsonStartup);
            if (startup.present)
            {
                if (startup.value.compare(JsonForeground) == 0)
                {
                    desiredState.startupType = StartupType::eForeground;
                    foreGroundStartupList.push_back(desiredState.packageFamilyName);
                }
                else if (startup.value.compare(JsonBackground) == 0)
                {
                    desiredState.startupType = StartupType::eBackground;
                }
                else if (startup.value.compare(JsonNone) == 0)
                {
                    desiredState.startupType = StartupType::eNone;
                    removeStartupList.push_back(desiredState.packageFamilyName);
                }
                else
                {
                    throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON, "Invalid startup type entry");
                }
            }

            // Check if the app is already installed
            desiredState.package = FindApp(desiredState.packageFamilyName);
            if (desiredState.package == nullptr)
            {
                desiredState.action = AppDesiredAction::eInstall;
            }
            else
            {
                Version installedVersion = Version(PackageVersion(desiredState.package));

                if (installedVersion == desiredState.version)
                {
                    desiredState.action = AppDesiredAction::eQuery;
                }
                else if (installedVersion < desiredState.version)
                {
                    desiredState.action = AppDesiredAction::eUpgrade;
                }
                else
                {
                    desiredState.action = AppDesiredAction::eDowngrade;
                }
            }

            // For install or upgrade, we need appx package and deps
            if (   desiredState.action == AppDesiredAction::eInstall 
                || desiredState.action == AppDesiredAction::eUpgrade
                || desiredState.action == AppDesiredAction::eDowngrade)
            {
                OperationModelT<string>  appxSource = Operation::TryGetStringJsonValue(*itr, JsonAppxSource);
                if (!appxSource.present)
                {
                    throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON, "App package source is missing");
                }
                else
                {
                    desiredState.appxSource = appxSource.value;
                }

                OperationModelT<string>  depsSource = Operation::TryGetStringJsonValue(*itr, JsonDepsSource);
                if (depsSource.present)
                {
                    desiredState.depsSources = depsSource.value;
                }

                OperationModelT<bool>  launchAfterInstall = Operation::TryGetBoolJsonValue(*itr, JsonLaunchAfterInstall);
                if (launchAfterInstall.present)
                {
                    desiredState.launchAfterInstall = launchAfterInstall.value;
                }
            }
            reorderedConfig.push_back(desiredState);
        }

        // Make sure that there is only one foreground app getting set
        if (foreGroundStartupList.size() > 1)
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_SHELL_ERROR_INVALID_DESIRED_MULTIPLE_FOREGROUND_APPS, "Multiple foreground apps specified in desired configuration");
        }

        // Make sure we are not uninstalling foreground app without setting a new one
        for (auto const& pkgFamilyName : uninstallList)
        {
            if (pkgFamilyName.compare(currentStartupApp) == 0)
            {
                if (foreGroundStartupList.size() == 0)
                {
                    throw DMException(DMSubsystem::DeviceAgent, DM_SHELL_ERROR_CONFLICT_UNINSTALL_FOREGROUND_APP, "Uninstalling foreground app without setting a different foreground app");
                }
            }
        }

        for (auto const& pkgFamilyName : removeStartupList)
        {
            // If this app is currently set as foreground app 
            if (pkgFamilyName.compare(currentStartupApp) == 0)
            {
                if (foreGroundStartupList.size() == 0)
                {
                    throw DMException(DMSubsystem::DeviceAgent, DM_SHEEL_ERROR_INVALID_DESIRED_NO_FOREGROUND_APPS, "One foreground app needs to be set.");
                }
            }
        }

        // Reorder to make sure Foreground app install | update happens first
        if (foreGroundStartupList.size() == 1)
        {
            // If the first entry is the new foreground app, no need to reorder
            if (reorderedConfig.at(0).startupType == StartupType::eForeground)
            {
                return;
            }

            for (int i = 1; i < reorderedConfig.size(); i++)
            {
                // Find the foreground app in the list
                if (reorderedConfig[i].packageFamilyName.compare(foreGroundStartupList[0]) == 0)
                {
                    // Swap out with first position
                    AppDesiredState temp = reorderedConfig[0];
                    reorderedConfig[0] = reorderedConfig[i];
                    reorderedConfig[i] = temp;
                }
            }
        }
    }

    const string UwpAppManagementHandler::DownloadAppPackageFromBlob(
        const string& connectionString,
        const string& containerAndBlob,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        vector<string> fileConfigurationParts;
        SplitString(containerAndBlob, FileConfigurationSeparator, fileConfigurationParts);
        if (fileConfigurationParts.size() < (int)2)
        {
            throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "File Name format is incorrect. It should be <ContainerName>\\<BlobName>");
        }

        wstring containerName = MultibyteToWide(fileConfigurationParts[0].c_str());
        wstring blobName = MultibyteToWide(containerAndBlob.substr(containerName.size() + 1).c_str());

        auto storageAccount = azure::storage::cloud_storage_account::parse(MultibyteToWide(connectionString.c_str()));
        auto blobClient = storageAccount.create_cloud_blob_client();

        auto container = blobClient.get_container_reference(containerName);

        concurrency::streams::container_buffer<vector<uint8_t>> buffer;
        concurrency::streams::ostream output_stream(buffer);
        auto binary_blob = container.get_block_blob_reference(blobName);
        binary_blob.download_to_stream(output_stream);

        string fullFileName = (WideToMultibyte(Shell::GetDmUserFolder().c_str()) + fileConfigurationParts[fileConfigurationParts.size() - 1]);

        TRACELINEP(LoggingLevel::Verbose, "App package downloaded at: ", fullFileName.c_str());

        ofstream outfile(fullFileName, ofstream::binary);
        vector<unsigned char>& data = buffer.collection();
        outfile.write(reinterpret_cast<char *>(&data[0]), buffer.size());
        outfile.close();

        return fullFileName;
    }

    void UwpAppManagementHandler::BuildReported(
        Json::Value& reportedObject,
        const std::vector<std::pair<Package^, StartupType>> &modifiedApps,
        const std::vector<std::string> &uninstalledApps)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Json::Value appList(Json::objectValue);
        for (auto installedApp : modifiedApps) 
        {
            Json::Value appInfo;
            string pkgFamilyName = Utils::WideToMultibyte(installedApp.first->Id->FamilyName->Data());
            appInfo[JsonPkgFamilyName] = Json::Value(pkgFamilyName);
            appInfo[JsonVersion] = Json::Value(PackageVersion(installedApp.first));
            appInfo[JsonAppName] = Json::Value(Utils::WideToMultibyte(installedApp.first->Id->Name->Data()));

            // BUGBUG: This code doesn't seems to work on IoT Core. Uncomment when underlying OS issue is fixed. 
            // String^ format = L"{year.full}-{month.integer(2)}-{day.integer(2)}T{hour.integer(2)}:{minute.integer(2)}:{second.integer(2)}";
            // DateTimeFormatter^ formatter = ref new DateTimeFormatter(format);
            // String^ installedDate = formatter->Format(installedApp.first->InstalledDate);
            // appInfo[JsonInstallDate] = Json::Value(WideToMultibyte(installedDate->Data()));

            switch (installedApp.second)
            {
            case StartupType::eNone:
                appInfo[JsonStartup] = Json::Value(JsonNone);
                break;
            case StartupType::eForeground:
                appInfo[JsonStartup] = Json::Value(JsonForeground);
                break;
            case StartupType::eBackground:
                appInfo[JsonStartup] = Json::Value(JsonBackground);
                break;
            }

            string pkgFamilyId = pkgFamilyName;
            replace(pkgFamilyId.begin(), pkgFamilyId.end(), '.', '_');
            appList[pkgFamilyId] = Json::Value(appInfo);
        }

        for (auto uninstalledApp : uninstalledApps)
        {
            Json::Value appInfo;
            appInfo[JsonPkgFamilyName] = Json::Value(uninstalledApp);
            appInfo[JsonVersion] = Json::Value(JsonNotInstalled);

            string pkgFamilyId = uninstalledApp;
            replace(pkgFamilyId.begin(), pkgFamilyId.end(), '.', '_');
            appList[pkgFamilyId] = Json::Value(appInfo);
        }

        reportedObject[JsonApps] = Json::Value(appList);
;   }

    void UwpAppManagementHandler::EmptyReported(
        Json::Value& reportedObject)
    {
        Json::Value nullValue;
        reportedObject[JsonApps] = nullValue;
    }

    std::string UwpAppManagementHandler::PackageVersion(Windows::ApplicationModel::Package^ package)
    {
        wstring wideVersion = to_wstring(package->Id->Version.Major) + L"." +
            to_wstring(package->Id->Version.Minor) + L"." +
            to_wstring(package->Id->Version.Build) + L"." +
            to_wstring(package->Id->Version.Revision);

        return Utils::WideToMultibyte(wideVersion.c_str());
    }

    Package^ UwpAppManagementHandler::FindApp(const string& packageFamilyName)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINEP(LoggingLevel::Verbose, L"Finding package: ", packageFamilyName.c_str());

        wstring widePackageFamilyName = MultibyteToWide(packageFamilyName.c_str());
        
        PackageManager^ packageManager = ref new PackageManager;
        IIterable<Package^>^ packages = packageManager->FindPackages();
        IIterator<Package^>^ it = packages->First();
        while (it->HasCurrent)
        {
            Package^ package = it->Current;

            TRACELINE(LoggingLevel::Verbose, "---------------------------------------------------------------------------");
            TRACELINEP(LoggingLevel::Verbose, "Name        : ", Utils::WideToMultibyte(package->Id->Name->Data()).c_str());
            TRACELINEP(LoggingLevel::Verbose, "DisplayName : ", Utils::WideToMultibyte(package->DisplayName->Data()).c_str());
            TRACELINEP(LoggingLevel::Verbose, "FullName    : ", Utils::WideToMultibyte(package->Id->FullName->Data()).c_str());
            TRACELINEP(LoggingLevel::Verbose, "FamilyName  : ", Utils::WideToMultibyte(package->Id->FamilyName->Data()).c_str());
            TRACELINEP(LoggingLevel::Verbose, "Name        : ", Utils::WideToMultibyte(package->Id->Name->Data()).c_str());
            TRACELINE(LoggingLevel::Verbose, "---------------------------------------------------------------------------");

            if (0 == _wcsicmp(widePackageFamilyName.c_str(), package->Id->FamilyName->Data()))
            {
                return package;
            }

            it->MoveNext();
        }

        TRACELINE(LoggingLevel::Verbose, "Not found.");
        return nullptr;
    }

    void UwpAppManagementHandler::InstallApp(
        Package^ package, 
        const string& packageFamilyName, const string& appxLocalPath,
        const vector<string>& dependentPackages,
        bool launchApp)
    {
        wstring widePackageFamilyName = Utils::MultibyteToWide(packageFamilyName.c_str());

        AutoCloseHandle completedEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        if (!completedEvent.Get())
        {
            TRACELINE(LoggingLevel::Error, "Error: failed to create synchronization event.");
            throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to create event.");
        }

        TRACELINE(LoggingLevel::Verbose, "Installing appx and its dependencies...");
        {
            Impersonator impersonator;
            impersonator.ImpersonateShellHost();

            Vector<Uri^>^ appxDepPkgs = ref new Vector<Uri^>();
            for (const string& depSource : dependentPackages)
            {
                auto fullpath = MultibyteToWide(depSource.c_str());
                appxDepPkgs->Append(ref new Uri(ref new String(fullpath.c_str())));
            }

            auto fullappxpath = MultibyteToWide(appxLocalPath.c_str());
            Uri^ packageUri = ref new Uri(ref new Platform::String(fullappxpath.c_str()));

            PackageManager^ packageManager = ref new PackageManager;

            auto installTask = packageManager->AddPackageAsync(packageUri, appxDepPkgs, DeploymentOptions::ForceApplicationShutdown);

            installTask->Completed = ref new AsyncOperationWithProgressCompletedHandler<DeploymentResult^, DeploymentProgress>(
                [&](IAsyncOperationWithProgress<DeploymentResult^, DeploymentProgress>^ operation, AsyncStatus)
            {
                TRACELINE(LoggingLevel::Verbose, "Firing 'install completed' event.");
                SetEvent(completedEvent.Get());
            });

            TRACELINE(LoggingLevel::Verbose, "Waiting for installing to complete...");
            WaitForSingleObject(completedEvent.Get(), INFINITE);

            TRACELINE(LoggingLevel::Verbose, "Install task completed.");
            if (installTask->Status == AsyncStatus::Completed)
            {
                TRACELINE(LoggingLevel::Verbose, "Succeeded.");
            }
            else if (installTask->Status == AsyncStatus::Started)
            {
                // This should never happen...
                throw DMException(DMSubsystem::DeviceAgent, DM_SHELL_ERROR_APP_INSTALL_FAILED, "Error: failed to wait for installation to complete.");
            }
            else if (installTask->Status == AsyncStatus::Error)
            {
                auto deploymentResult = installTask->GetResults();
                string context = Utils::WideToMultibyte(deploymentResult->ErrorText->Data());
                throw DMException(DMSubsystem::DeviceAgent, installTask->ErrorCode.Value, context.c_str());
            }
            else if (installTask->Status == AsyncStatus::Canceled)
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_SHELL_ERROR_APP_INSTALL_TASK_CANCELLED, "Error: application installation task cancelled.");
            }

            if (launchApp)
            {
                UwpHelpers::StartStopApp(packageFamilyName, true);
            }
        }
    }

    void UwpAppManagementHandler::UninstallApp(const std::string& pkgFamilyName)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINEP(LoggingLevel::Verbose, L"Uninstalling app: ", pkgFamilyName.c_str());

        // FindApp uses PackageManager:FindPackages which needs to be run
        // as admin, not as the Impersonated user
        Package^ package = FindApp(pkgFamilyName);
        if (!package)
        {
            TRACELINE(LoggingLevel::Verbose, "Warning: failed to find the specified package.");
            throw DMException(DMSubsystem::DeviceAgent, DM_SHELL_ERROR_APP_UNINSTALL_FAILED, "Application is not installed");
        }

        UninstallApp(package);
    }

    void UwpAppManagementHandler::UninstallApp(Windows::ApplicationModel::Package^ package)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Utils::AutoCloseHandle  completedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (!completedEvent.Get())
        {
            TRACELINE(LoggingLevel::Verbose, "Error: failed to create synchronization event.");
            throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to create synchronization event");
        }

        TRACELINE(LoggingLevel::Verbose, "Uninstalling appx...");

        {
            Impersonator impersonator;
            impersonator.ImpersonateShellHost();

            PackageManager^ packageManager = ref new PackageManager;
            auto uninstallTask = packageManager->RemovePackageAsync(package->Id->FullName);
            uninstallTask->Completed = ref new AsyncOperationWithProgressCompletedHandler<DeploymentResult^, DeploymentProgress>(
                [&](IAsyncOperationWithProgress<DeploymentResult^, DeploymentProgress>^ operation, AsyncStatus)
            {
                TRACELINE(LoggingLevel::Verbose, "Firing 'uninstall completed' event.");
                SetEvent(completedEvent.Get());
            });

            TRACELINE(LoggingLevel::Verbose, "Waiting for uninstalling to complete...");
            WaitForSingleObject(completedEvent.Get(), INFINITE);

            TRACELINE(LoggingLevel::Verbose, "Uninstall task completed.");
            if (uninstallTask->Status == AsyncStatus::Completed)
            {
                TRACELINE(LoggingLevel::Verbose, "Succeeded.");
            }
            else if (uninstallTask->Status == AsyncStatus::Started)
            {
                // This should never happen...
                throw DMException(DMSubsystem::DeviceAgent, DM_SHELL_ERROR_APP_UNINSTALL_FAILED, "Error: failed to wait for uninstallation to complete.");
            }
            else if (uninstallTask->Status == AsyncStatus::Error)
            {
                auto deploymentResult = uninstallTask->GetResults();
                string context = Utils::WideToMultibyte(deploymentResult->ErrorText->Data());
                throw DMException(DMSubsystem::DeviceAgent, uninstallTask->ErrorCode.Value, context.c_str());
            }
            else if (uninstallTask->Status == AsyncStatus::Canceled)
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_SHELL_ERROR_APP_INSTALL_TASK_CANCELLED, "Error: application installation task cancelled.");
            }
        }
    }
}}}}
