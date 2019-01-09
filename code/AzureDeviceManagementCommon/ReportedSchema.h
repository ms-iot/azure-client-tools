// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class ReportedSchema
    {
    public:
        ReportedSchema();

        ReportedSchema(
            std::string interfaceType,
            std::string tags,
            unsigned int maxInputVersion,
            unsigned int outputVersion);

        std::string GetInterfaceType() const { return _interfaceType; }
        bool HasTag(const std::string& tag) const;
        bool HasAnyTag(const std::set<std::string>& tags) const;
        unsigned int GetMaxInputVersion() const { return _maxInputVersion; }
        unsigned int GetOutputVersion() const { return _outputVersion; }

        Json::Value ToJsonValue() const;

        void FromJsonValue(const Json::Value& jsonValueObject);

    private:
        std::string _interfaceType;
        std::set<std::string> _tags;
        unsigned int _maxInputVersion;
        unsigned int _outputVersion;
    };

}}}}
