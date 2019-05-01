// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <sstream>
#include "PluginConstants.h"
#include "CrossBinaryRequest.h"
#include "PluginJsonConstants.h"

using namespace std;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    std::string CrossBinaryRequest::BuildRequest(
        const std::string& targetType,
        const std::string& targetId,
        const std::string& targetMethod,
        const Json::Value& targetParameters)
    {
        // ToDo: make this code resilient...
        Json::Value jsonObject(Json::ValueType::objectValue);
        jsonObject[JsonTargetType] = targetType;
        jsonObject[JsonTargetId] = targetId;
        jsonObject[JsonTargetMethod] = targetMethod;
        jsonObject[JsonTargetParameters] = targetParameters;

        return jsonObject.toStyledString();
    }

    CrossBinaryRequest CrossBinaryRequest::FromJson(
        const Json::Value& value)
    {
        if (value.isNull() || !value.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_REQUEST_JSON_SCHEMA, "Invalid request json schema.");
        }

        // ToDo: make this code resilient...
        CrossBinaryRequest pluginRequest;
        pluginRequest.targetType = value[JsonTargetType].asString();
        pluginRequest.targetId = value[JsonTargetId].asString();
        pluginRequest.targetMethod = value[JsonTargetMethod].asString();
        pluginRequest.targetParameters = value[JsonTargetParameters];

        return pluginRequest;
    }

    CrossBinaryRequest CrossBinaryRequest::FromJsonString(
        const std::string& value)
    {
        Json::CharReaderBuilder builder;
        istringstream jsonStream(value);
        Json::Value jsonInput;
        string errs;

        if (!Json::parseFromStream(builder, jsonStream, &jsonInput, &errs))
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_REQUEST_JSON, "Failed to parse plug-in request.");
        }

        return FromJson(jsonInput);
    }

}}}}
