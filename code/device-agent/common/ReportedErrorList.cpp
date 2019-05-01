// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ReportedErrorList.h"
#include "DMJsonConstants.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    ReportedErrorList::ReportedErrorList()
    {
    }

    size_t ReportedErrorList::Count() const
    {
        return _errorMap.size();
    }

    void ReportedErrorList::AddError(
        const std::string& operationId,
        std::shared_ptr<ReportedError> reportedError)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        _errorMap[operationId] = reportedError;
    }

    void ReportedErrorList::RemoveAll()
    {
        _errorMap.clear();
    }

    Json::Value ReportedErrorList::ToJsonObject() const
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Json::Value errorListObject;

        if (_errorMap.size() != 0)
        {
            errorListObject = Json::Value(Json::objectValue);
            for (ErrorMap::const_iterator it = _errorMap.cbegin(); it != _errorMap.cend(); ++it)
            {
                errorListObject[it->first] = it->second->ToJsonObject();
            }
        }

        return errorListObject;
    }

    std::string ReportedErrorList::ToJsonString() const
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Json::Value errorListObject = ToJsonObject();

        return errorListObject.toStyledString();
    }

}}}}
