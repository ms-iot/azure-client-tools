// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "BaseHandler.h"
#include "..\AzureDeviceManagementCommon\DMConstants.h"
#include "..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    BaseHandler::BaseHandler(
        const std::string& id,
        const ReportedSchema& reportedSchema) :
        _id(id),
        _isConfigured(false),
        _reportedSchema(reportedSchema),
        _iPluginHost(nullptr),
        _handlerConfig(Json::Value())
    {
        _metaData = make_shared<MetaData>();
        _metaData->SetDeploymentId(JsonDeploymentIdUnspecified);

        SetDeploymentStatus(DeploymentStatus::eNotStarted);
    }

    bool BaseHandler::IsRefreshing(
        const Json::Value& desiredConfig) const
    {
        if (desiredConfig.isNull())
        {
            throw DMException(ErrorInvalidJsonFormat);
        }

        if (desiredConfig.isString())
        {
            return desiredConfig.asString() == JsonRefreshing;
        }

        return false;
    }

    std::string BaseHandler::GetHandlerType() const
    {
        return JsonHandlerTypeRaw;
    }

    string BaseHandler::GetId() const
    {
        return _id;
    }

    bool BaseHandler::IsConfigured() const
    {
        return _isConfigured;
    }

    void BaseHandler::FinalizeAndReport(
        Json::Value& reportedObject,
        std::shared_ptr<ReportedErrorList> errorList)
    {
        DeploymentStatus deploymentStatus = errorList->Count() == 0 ? DeploymentStatus::eSucceeded : DeploymentStatus::eFailed;

        _metaData->SetDeploymentStatus(deploymentStatus);
        reportedObject[JsonMeta] = _metaData->ToJsonObject();
        reportedObject[JsonErrorList] = errorList->ToJsonObject();

        _iPluginHost->Report(GetId().c_str(), deploymentStatus, reportedObject);
    }

    void BaseHandler::SignalRefreshing()
    {
        _iPluginHost->Report(GetId().c_str(), DeploymentStatus::ePending, Json::Value(JsonRefreshing));
    }

    void BaseHandler::SetConfig(
        const Json::Value& handlerConfig)
    {
        _handlerConfig = handlerConfig;
    }

    Json::Value BaseHandler::GetConfig() const
    {
        return _handlerConfig;
    }

    void BaseHandler::Stop()
    {
        // default implementation.
    }

    DeploymentStatus BaseHandler::GetDeploymentStatus() const
    {
        return _metaData->GetDeploymentStatus();
    }

    void BaseHandler::SetDeploymentStatus(
        DeploymentStatus deploymentStatus)
    {
        _metaData->SetDeploymentStatus(deploymentStatus);

        Json::Value root(Json::objectValue);
        root[JsonMeta] = _metaData->ToJsonObject(JsonDeploymentStatus);
        _deploymentStatusJson = root;
    }

    Json::Value BaseHandler::GetDeploymentStatusJson() const
    {
        return _deploymentStatusJson;
    }

    ReportedSchema BaseHandler::GetReportedSchema() const
    {
        return _reportedSchema;
    }

    OperationModel BaseHandler::TryGetOptionalSinglePropertyOpParameter(
        const Json::Value& groupRoot,
        const std::string& subGroupdId)
    {
        TRACELINEP(LoggingLevel::Verbose, "properties to search = ", groupRoot.toStyledString().c_str());
        TRACELINEP(LoggingLevel::Verbose, "property to find = ", subGroupdId.c_str());

        OperationModel operationDataModel;

        if (groupRoot.isNull())
        {
            return operationDataModel;
        }

        if (groupRoot.isString())
        {
            string s = groupRoot.asString();
            if (s == JsonRefreshing)
            {
                return operationDataModel;
            }
            throw DMException(ErrorInvalidJsonFormat);
        }

        if (!groupRoot.isObject())
        {
            throw DMException(ErrorInvalidJsonFormat);
        }

        Json::Value value = groupRoot[subGroupdId];
        if (!value.isNull())
        {
            operationDataModel.present = true;
            operationDataModel.value = value;
            return operationDataModel;
        }

        return operationDataModel;
    }

    OperationModelT<int> BaseHandler::TryGetOptionalSinglePropertyOpIntParameter(
        const Json::Value& groupRoot,
        const std::string& operationId)
    {
        OperationModelT<int> typedModel;
        OperationModel model = TryGetOptionalSinglePropertyOpParameter(groupRoot, operationId);

        typedModel.present = model.present;
        if (model.present)
        {
            if (!model.value.isInt())
            {
                throw DMException(ErrorInvalidJsonFormat);
            }

            typedModel.value = model.value.asInt();
        }

        return typedModel;
    }

    OperationModelT<bool> BaseHandler::TryGetOptionalSinglePropertyOpBoolParameter(
        const Json::Value& groupRoot,
        const std::string& operationId)
    {
        OperationModelT<bool> typedModel;
        OperationModel model = TryGetOptionalSinglePropertyOpParameter(groupRoot, operationId);

        typedModel.present = model.present;
        if (model.present)
        {
            if (!model.value.isBool())
            {
                throw DMException(ErrorInvalidJsonFormat);
            }

            typedModel.value = model.value.asBool();
        }

        return typedModel;
    }

    OperationModelT<std::string> BaseHandler::TryGetOptionalSinglePropertyOpStringParameter(
        const Json::Value& groupRoot,
        const std::string& operationId)
    {
        OperationModelT<std::string> typedModel;
        OperationModel model = TryGetOptionalSinglePropertyOpParameter(groupRoot, operationId);

        typedModel.present = model.present;
        if (model.present)
        {
            if (!model.value.isString())
            {
                throw DMException(ErrorInvalidJsonFormat);
            }

            typedModel.value = model.value.asString();
        }

        return typedModel;
    }

    string BaseHandler::GetSinglePropertyOpStringParameter(
        const Json::Value& groupRoot,
        const string& operationId)
    {
        OperationModelT<string> model = TryGetOptionalSinglePropertyOpStringParameter(groupRoot, operationId);
        if (!model.present)
        {
            throw DMException(ErrorInvalidJsonFormat);
        }
        return model.value;
    }

    bool BaseHandler::RunOperation(
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
        catch (...)
        {
            const char* displayMessage = "Generic exception occured.";
            TRACELINE(LoggingLevel::Error, displayMessage);

            reportedError = make_shared<ReportedError>();
            reportedError->SetContext(subGroupId);
            reportedError->SetSubsystem(JsonErrorSubsystemUnknown);
            reportedError->SetCode(JsonErrorGenericCode);
            reportedError->SetMessage(displayMessage);
        }

        if (!result)
        {
            errorList->AddError(subGroupId, reportedError);
        }

        return result;
    }

    void BaseHandler::SetMdmServer(
        std::shared_ptr<IMdmServer> iMdmServer)
    {
        _mdmProxy.SetMdmServer(iMdmServer);
    }

    void BaseHandler::SetHandlerHost(
        std::shared_ptr<IRawHandlerHost> iPluginHost)
    {
        _iPluginHost = iPluginHost;
    }

}}}}
