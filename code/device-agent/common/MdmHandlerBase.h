// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "HandlerBase.h"
#include "CSPs\MdmProxy.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class MdmHandlerBase : public HandlerBase
    {
    public:

        MdmHandlerBase(
            const std::string& id,
            const ReportedSchema& reportedSchema);

        void SetHandlerHost(
            std::shared_ptr<IRawHandlerHost> iPluginHost);

    protected:
        MdmProxy _mdmProxy;
    };

}}}}