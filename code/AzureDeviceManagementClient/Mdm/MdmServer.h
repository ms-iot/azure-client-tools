// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "..\..\AzureDeviceManagementCommon\DMInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class MdmServer : public DMCommon::IMdmServer
    {
        MdmServer();
    public:
        static std::shared_ptr<DMCommon::IMdmServer> GetInstance();

        std::string RunSyncML(const std::string& sid, const std::string& syncML);

    private:
        friend std::_Ref_count_obj<MdmServer>;

        static std::shared_ptr<MdmServer> _this;
        static std::mutex _lock;
    };

}}}}
