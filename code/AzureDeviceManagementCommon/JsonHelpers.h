// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "DMInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class JsonHelpers
    {
    public:
        static bool HasKey(const Json::Value& object, const std::string& name);
        static int GetNamedInteger(const Json::Value& object, const std::string& name);
        static std::string GetNamedString(const Json::Value& root, const std::string& name);
        static std::string GetNamedString(const Json::Value& root, const std::string& name, const std::string& defaultValue);

        static void Merge(const Json::Value& source, Json::Value& target);

        static std::string DeploymentStatusToString(DeploymentStatus deploymentState);

    };

}}}}
