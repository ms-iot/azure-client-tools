// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DMJsonConstants.h"
#include "ReportedSchema.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    ReportedSchema::ReportedSchema()
    {}

    ReportedSchema::ReportedSchema(
            string interfaceType,
            string tags,
            string interfaceVersion) :
            _interfaceType(interfaceType),
            _interfaceVersion(interfaceVersion)
    {
        vector<string> tagsVector;
        Utils::SplitString(tags, ';', tagsVector);
        for (const auto& t : tagsVector)
        {
            _tags.emplace(t);
        }
    }

    bool ReportedSchema::HasTag(const string& tag) const
    {
        return _tags.find(tag) != _tags.cend();
    }

    bool ReportedSchema::HasAnyTag(const set<string>& tags) const
    {
        for (const string& t : tags)
        {
            if (_tags.cend() != _tags.find(t))
            {
                return true;
            }
        }

        return false;
    }

    Json::Value ReportedSchema::ToJsonValue() const
    {
        Json::Value object(Json::objectValue);

        object[JsonDeviceSchemaType] = Json::Value(_interfaceType);

        string tags;
        for (const auto& tag : _tags)
        {
            if (tags.size() != 0)
            {
                tags += ";";
            }
            tags += tag;
        }
        object[JsonDeviceSchemasTags] = Json::Value(tags);

        object[JsonDeviceSchemasInterfaceVersion] = Json::Value(_interfaceVersion);

        return object;
    }

    void ReportedSchema::FromJsonValue(const Json::Value& jsonValueObject)
    {
        _interfaceType = jsonValueObject[JsonDeviceSchemaType].asString();
        string tags = jsonValueObject[JsonDeviceSchemasTags].asString();
        _interfaceVersion = jsonValueObject[JsonDeviceSchemasInterfaceVersion].asString();
    }

}}}}