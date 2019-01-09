// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

#include "..\..\AzureDeviceManagementCommon\DMInterfaces.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginInterfaces.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\CrossBinaryResponse.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class OutOfProcPluginBinaryProxy : public DMCommon::IPluginBinaryProxy
    {
    public:
        OutOfProcPluginBinaryProxy(const std::string& pluginFullPath, long keepAliveTime);

        void Load();

        Json::Value Invoke(
            const std::string& targetType,
            const std::string& targetId,
            const std::string& targetMethod,
            const Json::Value& targetParameters) const;

        void Unload();

    private:
        void CreateRawHandler(const std::string& id) const;
        std::shared_ptr<DMCommon::IPluginTransport> _transport;
    };
}}}}
