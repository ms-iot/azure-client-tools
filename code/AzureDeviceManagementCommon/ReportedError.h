// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <map>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class ReportedError
    {
        typedef std::map<std::string, std::string> ParametersMap;

    public:
        ReportedError();

        void SetSubsystem(
            const std::string& subsystem);

        void SetContext(
            const std::string& context);

        void SetCode(
            int code);

        void SetMessage(
            const std::string& message);

        void AddParameters(
            const std::string& name,
            const std::string& value);

        Json::Value ToJsonObject() const;

        std::string ToJsonString() const;

    private:

        std::string _subsystem;
        std::string _context;
        int _code;
        std::string _message;
        ParametersMap _parameters;

    };

}}}}
