// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <set>
#include "Utilities\json\json.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class ReportedSchema
    {
    public:
        ReportedSchema();

        ReportedSchema(
            std::string interfaceType,
            std::string tags,
            std::string interfaceVersion);

        std::string GetInterfaceType() const { return _interfaceType; }
        bool HasTag(const std::string& tag) const;
        bool HasAnyTag(const std::set<std::string>& tags) const;
        std::string GetInterfaceVersion() const { return _interfaceVersion; }

        Json::Value ToJsonValue() const;

        void FromJsonValue(const Json::Value& jsonValueObject);

    private:
        std::string _interfaceType;
        std::set<std::string> _tags;
        std::string _interfaceVersion;
    };

}}}}
