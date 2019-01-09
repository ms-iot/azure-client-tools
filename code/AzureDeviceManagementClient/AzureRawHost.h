// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <string>
#include "..\AzureDeviceManagementCommon\DMInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    enum RawHandlerState
    {
        eInactive,
        eActive
    };

    class RawHandlerInfo
    {
    public:
        RawHandlerInfo() :
            _state(eInactive)
        {}

        RawHandlerState _state;
        std::shared_ptr<DMCommon::IRawHandler> _rawHandler;
    };

    typedef std::map<std::string, std::shared_ptr<RawHandlerInfo>> RawHandlerMapType;

}}}}
