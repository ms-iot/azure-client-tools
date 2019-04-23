// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ReportedError.h"
#include "DMJsonConstants.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    ReportedError::ReportedError() :
        _subsystem(JsonErrorSubsystemUnknown),
        _code(0)
    {
    }

    void ReportedError::SetSubsystem(
        const string& subsystem)
    {
        _subsystem = subsystem;
    }

    void ReportedError::SetContext(
        const std::string& context)
    {
        _context = context;
    }

    void ReportedError::SetCode(
        int code)
    {
        _code = code;
    }

    void ReportedError::SetMessage(
        const std::string& message)
    {
        _message = message;
    }

    void ReportedError::AddParameters(
        const std::string& name,
        const std::string& value)
    {
        _parameters[name] = value;
    }

    Json::Value ReportedError::ToJsonObject() const
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Json::Value parametersObject(Json::objectValue);
        for (ParametersMap::const_iterator it = _parameters.cbegin(); it != _parameters.cend(); ++it)
        {
            parametersObject[it->first.c_str()] = Json::Value(it->second.c_str());
        }

        Json::Value errorObject(Json::objectValue);
        errorObject[JsonErrorSubsystem] = Json::Value(_subsystem);
        errorObject[JsonErrorContext] = Json::Value(_context);
        errorObject[JsonErrorCode] = Json::Value(_code);
        errorObject[JsonErrorMessage] = Json::Value(_message);
        errorObject[JsonErrorParameters] = parametersObject;
        return errorObject;
    }

    std::string ReportedError::ToJsonString() const
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Json::Value errorObject = ToJsonObject();

        return errorObject.toStyledString();
    }

}}}}
