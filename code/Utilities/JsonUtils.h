// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "json/json.h"
#include <string>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class JsonUtils
    {
    public:
        static Json::Value ParseJSONFile(
            const std::string& fileName);

        static Json::Value JsonObjectFromString(
            const std::string& value);

        static bool TryParseJSONFile(
            const std::string& fileName, Json::Value& readJson);
    };

}}}}
