// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "MdmHandlerBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    MdmHandlerBase::MdmHandlerBase(
        const std::string& id,
        const ReportedSchema& reportedSchema) :
        HandlerBase(id, reportedSchema)
    {
    }

    void MdmHandlerBase::SetHandlerHost(
        std::shared_ptr<IRawHandlerHost> iPluginHost)
    {
        HandlerBase::SetHandlerHost(iPluginHost);
        _mdmProxy.SetMdmServer(iPluginHost->GetMdmServer());
    }

}}}}