// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Operation.h"
#include "DMConstants.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    OperationModel Operation::TryGetJsonValue(
        const Json::Value& jsonObject,
        const std::string& propertyName)
    {
        TRACELINEP(LoggingLevel::Verbose, "properties to search = ", jsonObject.toStyledString().c_str());
        TRACELINEP(LoggingLevel::Verbose, "property to find = ", propertyName.c_str());

        OperationModel operationDataModel;

        if (jsonObject.isNull())
        {
            return operationDataModel;
        }

        if (jsonObject.isString())
        {
            string s = jsonObject.asString();
            if (s == JsonRefreshing)
            {
                return operationDataModel;
            }
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Group root is invalid");
        }

        if (!jsonObject.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Group root is not a valid object");
        }

        Json::Value value = jsonObject[propertyName];
        if (!value.isNull())
        {
            operationDataModel.present = true;
            operationDataModel.value = value;
            return operationDataModel;
        }

        return operationDataModel;
    }

    OperationModelT<int> Operation::TryGetOptionalSinglePropertyOpIntParameter(
        const Json::Value& groupRoot,
        const std::string& operationId)
    {
        OperationModelT<int> typedModel;
        OperationModel model = TryGetJsonValue(groupRoot, operationId);

        typedModel.present = model.present;
        if (model.present)
        {
            if (!model.value.isInt())
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Property value is not of integer type");
            }

            typedModel.value = model.value.asInt();
        }

        return typedModel;
    }

    OperationModelT<bool> Operation::TryGetOptionalSinglePropertyOpBoolParameter(
        const Json::Value& groupRoot,
        const std::string& operationId)
    {
        OperationModelT<bool> typedModel;
        OperationModel model = TryGetJsonValue(groupRoot, operationId);

        typedModel.present = model.present;
        if (model.present)
        {
            if (!model.value.isBool())
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Property value is not of boolean type");
            }

            typedModel.value = model.value.asBool();
        }

        return typedModel;
    }

    OperationModelT<std::string> Operation::TryGetOptionalSinglePropertyOpStringParameter(
        const Json::Value& groupRoot,
        const std::string& operationId)
    {
        OperationModelT<std::string> typedModel;
        OperationModel model = TryGetJsonValue(groupRoot, operationId);

        typedModel.present = model.present;
        if (model.present)
        {
            if (!model.value.isString())
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Property value is not of string type");
            }

            typedModel.value = model.value.asString();
        }

        return typedModel;
    }

    string Operation::GetSinglePropertyOpStringParameter(
        const Json::Value& groupRoot,
        const string& operationId)
    {
        OperationModelT<string> model = TryGetOptionalSinglePropertyOpStringParameter(groupRoot, operationId);
        if (!model.present)
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Property not found");
        }
        return model.value;
    }

    bool Operation::RunOperation(
        const string& subGroupId,
        shared_ptr<ReportedErrorList> errorList,
        const function<void()>& Action)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINEP(LoggingLevel::Verbose, "Sub-Group: ", subGroupId.c_str());

        bool result = false;
        std::shared_ptr<ReportedError> reportedError;

        try
        {
            Action();
            result = true;
        }
        catch (const DMException& e)
        {
            TRACELINE(LoggingLevel::Error, e.DisplayMessage().c_str());

            reportedError = make_shared<ReportedError>();
            reportedError->SetContext(subGroupId);
            reportedError->SetSubsystem(e.SubSystem());
            reportedError->SetCode(e.Code());
            reportedError->SetMessage(e.Message());
        }
        catch (const exception& e)
        {
            string message;
            message += "Generic standard exception occured: ";
            message += e.what();

            TRACELINE(LoggingLevel::Error, message.c_str());

            reportedError = make_shared<ReportedError>();
            reportedError->SetContext(subGroupId);
            reportedError->SetSubsystem(JsonErrorSubsystemUnknown);
            reportedError->SetCode(JsonErrorGenericCode);
            reportedError->SetMessage(e.what());
        }

        if (!result)
        {
            errorList->AddError(subGroupId, reportedError);
        }

        return result;
    }

    bool Operation::IsRefreshing(
        const Json::Value& desiredConfig)
    {
        if (desiredConfig.isNull())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Desired configuration is Null");
        }

        if (desiredConfig.isString())
        {
            return desiredConfig.asString() == JsonRefreshing;
        }

        return false;
    }

}}}}