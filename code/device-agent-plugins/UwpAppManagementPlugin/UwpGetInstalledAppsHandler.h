// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include "device-agent/common/MdmHandlerBase.h"
#include "AppInfo.h"

typedef std::map<std::string, std::shared_ptr<ApplicationInfo>> ApplicationsMap;

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
            const std::string store,
            ApplicationsMap &installedAppsList,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void BuildReported(
            Json::Value& reportedObject,
            bool store,
            bool nonStore,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);
    };

}}}}
