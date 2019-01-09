// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CrossBinaryResponse.h"
#include "PluginJsonConstants.h"

using namespace std;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {


    CrossBinaryResponse::CrossBinaryResponse() :
        success(true)
    {}

    CrossBinaryResponse CrossBinaryResponse::CreateFromException(const DMUtils::DMException& ex)
    {
        CrossBinaryResponse cbr;
        cbr.success = false;
        cbr.payload = Json::Value(Json::ValueType::objectValue);
        cbr.payload[JsonCrossBinaryErrorType] = Json::Value(JsonCrossBinaryErrorTypeDMException);
        cbr.payload[JsonCrossBinaryErrorSubSystem] = Json::Value(ex.SubSystem());
        cbr.payload[JsonCrossBinaryErrorCode] = Json::Value(ex.Code());
        cbr.payload[JsonCrossBinaryErrorMessage] = Json::Value(ex.Message());
        return cbr;
    }

    CrossBinaryResponse CrossBinaryResponse::CreateFromException(const std::exception& ex)
    {
        CrossBinaryResponse cbr;
        cbr.success = false;
        cbr.payload = Json::Value(Json::ValueType::objectValue);
        cbr.payload[JsonCrossBinaryErrorType] = Json::Value(JsonCrossBinaryErrorTypeStdException);
        cbr.payload[JsonCrossBinaryErrorMessage] = Json::Value(ex.what());
        return cbr;
    }

    CrossBinaryResponse CrossBinaryResponse::CreateFromException()
    {
        CrossBinaryResponse cbr;
        cbr.success = false;
        cbr.payload = Json::Value(Json::ValueType::objectValue);
        cbr.payload[JsonCrossBinaryErrorType] = Json::Value(JsonCrossBinaryErrorTypeStdException);
        cbr.payload[JsonCrossBinaryErrorMessage] = Json::Value(JsonCrossBinaryErrorMessageUnknown);
        return cbr;
    }

    CrossBinaryResponse CrossBinaryResponse::CreateFromSuccess(const Json::Value& payload_)
    {
        CrossBinaryResponse cbr;
        cbr.success = true;
        cbr.payload = payload_;
        return cbr;
    }

    Json::Value CrossBinaryResponse::GetPayloadFromJsonString(const std::string& responseJsonString)
    {
        Json::CharReaderBuilder builder;
        istringstream jsonStream(responseJsonString);
        Json::Value jsonObject;
        string errs;

        if (!Json::parseFromStream(builder, jsonStream, &jsonObject, &errs))
        {
            throw DMException(PLUGIN_ERR_INVALID_RESPONSE_JSON, "Failed to parse json to construct CrossBinaryResponse.");
        }

        if (jsonObject.isNull() || !jsonObject.isObject())
        {
            throw DMException(PLUGIN_ERR_INVALID_RESPONSE_JSON_SCHEMA, "Failed to parse plug-in response: invalid type.");
        }

        Json::Value successValue = jsonObject[JsonCrossBinaryResponseSuccess];
        if (successValue.isNull() || !successValue.isBool())
        {
            throw DMException(PLUGIN_ERR_INVALID_RESPONSE_JSON_SCHEMA, "Failed to parse plug-in response: invalid type or missing 'success'.");
        }

        CrossBinaryResponse cbr;
        cbr.success = jsonObject[JsonCrossBinaryResponseSuccess].asBool();
        cbr.payload = jsonObject[JsonCrossBinaryResponsePayload];

        if (!cbr.success)
        {
            string errorType = cbr.payload[JsonCrossBinaryErrorType].asString();
            if (errorType == JsonCrossBinaryErrorTypeDMException)
            {
                ThrowDMException(cbr.payload);
            }
            else if (errorType == JsonCrossBinaryErrorTypeStdException)
            {
                ThrowStdException(cbr.payload);
            }
        }

        return cbr.payload;
    }

    void CrossBinaryResponse::ThrowDMException(const Json::Value& payload)
    {
        DMException ex(
            payload[JsonCrossBinaryErrorSubSystem].asString(),
            payload[JsonCrossBinaryErrorCode].asInt(),
            payload[JsonCrossBinaryErrorMessage].asString());
        throw ex;
    }

    void CrossBinaryResponse::ThrowStdException(const Json::Value& payload)
    {
        std::exception ex(
            payload[JsonCrossBinaryErrorMessage].asString().c_str());

        throw ex;
    }

    Json::Value CrossBinaryResponse::ToJson() const
    {
        Json::Value value(Json::ValueType::objectValue);
        value[JsonCrossBinaryResponseSuccess] = Json::Value(success);
        value[JsonCrossBinaryResponsePayload] = payload;
        return value;
    }

}}}}
