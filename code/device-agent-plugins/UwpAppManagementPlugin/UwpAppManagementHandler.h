// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include "device-agent/common/MdmHandlerBase.h"
#include "UwpHelpers.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace UwpAppManagementPlugin {

    class UwpAppManagementHandler : public DMCommon::MdmHandlerBase
    {
    public:
        UwpAppManagementHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredConfig) noexcept;

    private:
        void ModifyApplicationHandler(
            const Json::Value& desiredConfig,
            const DMCommon::OperationModelT<std::string>& connectionString,
            std::vector<std::pair<Windows::ApplicationModel::Package^, StartupType>> &modifiedApps,
            std::vector<std::string> &uninstalledApps,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void ReorderAndValidate(const Json::Value& desiredConfig, AppDesiredStateList& reorderedConfig);

        void BuildReported(
            Json::Value& reportedObject,
            const std::vector<std::pair<Windows::ApplicationModel::Package^, StartupType>> &modifiedApps,
            const std::vector<std::string> &uninstalledApps);

        void EmptyReported(
            Json::Value& reportedObject);

        const std::string DownloadAppPackageFromBlob(
            const std::string& connectionString,
            const std::string& containerAndBlob,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        Windows::ApplicationModel::Package^ FindApp(
            const std::string& packageFamilyName);
        
        void UninstallApp(
            const std::string& packageFamilyName);

        void UninstallApp(
            Windows::ApplicationModel::Package^);
        
        void InstallApp(
            Windows::ApplicationModel::Package^,
            const std::string& packageFamilyName,
            const std::string& appxLocalPath,
            const std::vector<std::string>& dependentPackages,
            bool launchApp);

        std::string PackageVersion(
            Windows::ApplicationModel::Package^);
    };

}}}}
