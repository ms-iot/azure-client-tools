// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include "device-agent/common/MdmHandlerBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace UwpAppManagementPlugin {

    class UwpGetInstalledAppsHandler : public DMCommon::MdmHandlerBase
    {
    public:
        UwpGetInstalledAppsHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredConfig) noexcept;

    private:
        void GetInstalledApps(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        std::string PackageVersion(Windows::ApplicationModel::Package^ package)
        {
            std::wstring wideVersion = std::to_wstring(package->Id->Version.Major) + L"." +
                std::to_wstring(package->Id->Version.Minor) + L"." +
                std::to_wstring(package->Id->Version.Build) + L"." +
                std::to_wstring(package->Id->Version.Revision);

            return Utils::WideToMultibyte(wideVersion.c_str());
        }
    };

}}}}
