// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "PluginConstants.h"
#include "..\DMJsonConstants.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class CrossBinaryResponse
    {
    public:
        bool success;
        Json::Value payload;

        CrossBinaryResponse();

        static CrossBinaryResponse CreateFromException(
            const DMUtils::DMException& ex);

        static CrossBinaryResponse CreateFromException(
            const std::exception& ex);

        static CrossBinaryResponse CreateFromException();

        static CrossBinaryResponse CreateFromSuccess(
            const Json::Value& payload_);

        static Json::Value GetPayloadFromJsonString(
            const std::string& responseJsonString);

        static void ThrowDMException(
            const Json::Value& payload);

        static void ThrowStdException(
            const Json::Value& payload);

        Json::Value ToJson() const;
    };

}}}}
