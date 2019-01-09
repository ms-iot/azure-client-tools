// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class IDeviceTwin
    {
    public:
        virtual void Report(const Json::Value& partialReportedPropertiesObject) = 0;
        virtual void Report(const std::string& partialReportedPropertiesString) = 0;
        virtual void GetRegisteredHandlerNames(std::vector<std::string>& sectionNames) = 0;
    };

}}}}