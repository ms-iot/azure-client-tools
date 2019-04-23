// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class CrossBinaryRequest
    {
    public:
        std::string targetType;
        std::string targetId;
        std::string targetMethod;
        Json::Value targetParameters;

        static std::string BuildRequest(
            const std::string& targetType,
            const std::string& targetId,
            const std::string& targetMethod,
            const Json::Value& targetParameters);

        static CrossBinaryRequest FromJson(
            const Json::Value& value);

        static CrossBinaryRequest FromJsonString(
            const std::string& value);
    };

}}}}
