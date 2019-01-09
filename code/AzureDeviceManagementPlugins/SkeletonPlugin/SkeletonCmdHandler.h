// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Windows { namespace Azure { namespace DeviceManagement { namespace SkeletonPlugin {

    class SkeletonCmdHandler : public DMCommon::BaseHandler
    {
    public:
        SkeletonCmdHandler();

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredConfig) noexcept;
    };

}}}}}
