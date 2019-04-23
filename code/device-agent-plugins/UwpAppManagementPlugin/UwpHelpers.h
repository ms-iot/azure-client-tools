// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <stdafx.h>
#include "AppDesiredState.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace UwpAppManagementPlugin {

    class UwpHelpers
    {
    public:
        static void StartStopApp(
            const std::string& pkgFamilyName, bool start);

        static void UpdateAppStartup(
            const std::string& pkgFamilyName, StartupType);

        static std::string GetStartupApp();

    private:
        static const std::wstring IotStartupExePath();
    };
}}}}
