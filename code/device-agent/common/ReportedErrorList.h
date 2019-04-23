// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <map>
#include <memory>
#include <string>
#include "ReportedError.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class ReportedErrorList
    {

        typedef std::map<std::string, std::shared_ptr<ReportedError>> ErrorMap;

    public:
        ReportedErrorList();

        size_t Count() const;

        void AddError(
            const std::string& operationId,
            std::shared_ptr<ReportedError> reportedError);

        void RemoveAll();

        Json::Value ToJsonObject() const;

        std::string ToJsonString() const;

    private:
        ErrorMap _errorMap;
    };

}}}}
