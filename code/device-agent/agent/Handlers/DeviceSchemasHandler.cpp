// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "../DMConstants.h"
#include "../../common/plugins/PluginConstants.h"
#include "DeviceSchemasHandler.h"

using namespace Microsoft::Azure::DeviceManagement::Common;
using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    DeviceSchemasHandler::DeviceSchemasHandler(
        const RawHandlerMapType* rawHandlerMap) :
        HandlerBase(JsonDeviceSchemas, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1)),
        _rawHandlerMap(rawHandlerMap)
    {
        assert(_rawHandlerMap);

        _reportingModel.groupsModel.all = true;;
        _reportingModel.types.all = true;

        _groupDesiredConfigJson = Json::Value(Json::objectValue);
    }

    DeviceSchemasHandler::GroupsModel DeviceSchemasHandler::GroupsModel::FromJson(const Json::Value& parent)
    {
        Json::Value groupsValue = parent[JsonDeviceSchemasGroups];

        if (groupsValue.isNull() || !groupsValue.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Missing groups value");
        }

        string groupsString = groupsValue.asString();
        set<string> groups;
        Utils::SplitString(groupsString, ';', groups);

        GroupsModel groupsModel;

        // "groups"="all"
        if (groups.cend() != groups.find(JsonDeviceSchemasAll))
        {
            if (groups.size() != 1)
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Missing all group in device schema");
            }
            groupsModel.all = true;
        }
        else
        {
            for (const string& group : groups)
            {
                // "groups"="configured"
                if (group == JsonDeviceSchemasConfigured)
                {
                    groupsModel.configured = true;
                }
                else
                {
                    groupsModel.listedGroups.emplace(group);
                }
            }
        }

        return groupsModel;
    }

    DeviceSchemasHandler::TypesModel DeviceSchemasHandler::TypesModel::FromJson(const Json::Value& parent)
    {
        Json::Value typesValue = parent[JsonDeviceSchemasTypes];

        if (typesValue.isNull() || !typesValue.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Missing types value in device schema");
        }

        string typesString = typesValue.asString();
        set<string> types;
        Utils::SplitString(typesString, ';', types);

        TypesModel typesModel;

        // "groups"="all"
        if (types.cend() != types.find(JsonDeviceSchemasAll))
        {
            if (types.size() != 1)
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Missing all group in device schema");
            }
            typesModel.all = true;
        }
        else
        {
            typesModel.listedTypes = types;
        }

        return typesModel;
    }

    DeviceSchemasHandler::TagsModel DeviceSchemasHandler::TagsModel::FromJson(const Json::Value& parent)
    {
        Json::Value tagsValue = parent[JsonDeviceSchemasTags];

        if (tagsValue.isNull() || !tagsValue.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Missing tags value");
        }

        string tagsString = tagsValue.asString();
        set<string> types;
        Utils::SplitString(tagsString, ';', types);

        TagsModel tagsModel;

        tagsModel.listedTags = types;

        return tagsModel;
    }

    DeviceSchemasHandler::ReportingModel DeviceSchemasHandler::ReportingModel::FromJson(const Json::Value& parent)
    {
        Json::Value reportingValue = parent[JsonDeviceSchemasReporting];

        if (reportingValue.isNull() || !reportingValue.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON_FORMAT, "Missing reporting value");
        }

        ReportingModel reportingModel;
        reportingModel.groupsModel = GroupsModel::FromJson(reportingValue);
        reportingModel.types = TypesModel::FromJson(reportingValue);
        reportingModel.tags = TagsModel::FromJson(reportingValue);
        return reportingModel;
    }

    void DeviceSchemasHandler::Start(
        const Json::Value& handlerConfig,
        bool& active)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        SetConfig(handlerConfig);
        active = true;
    }

    void DeviceSchemasHandler::OnConnectionStatusChanged(
        ConnectionStatus status)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        if (status == ConnectionStatus::eOffline)
        {
            TRACELINE(LoggingLevel::Verbose, "Connection Status: Offline.");
        }
        else
        {
            TRACELINE(LoggingLevel::Verbose, "Connection Status: Online.");

            Json::Value handlerConfig = GetConfig();

            // Report on connect...
            Json::Value reportOnConnect = handlerConfig[JsonHandlerConfigReportOnConnect];
            if (!reportOnConnect.isNull() && reportOnConnect.isBool())
            {
                if (reportOnConnect.asBool())
                {
                    TRACELINE(LoggingLevel::Verbose, "Report on connect is on...");

                    Json::Value reportedObject(Json::objectValue);
                    std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();
                    BuildReported(reportedObject, errorList);
                    FinalizeAndReport(reportedObject, errorList);
                }
                else
                {
                    TRACELINE(LoggingLevel::Verbose, "Report on connect is off...");
                }
            }
        }
    }

    void DeviceSchemasHandler::SetSubGroup(
        const Json::Value& groupRoot)
    {
        _reportingModel = ReportingModel::FromJson(groupRoot);
        _isConfigured = true;
    }

    bool DeviceSchemasHandler::IsGroupIncluded(
        const GroupsModel& groupsModel,
        const IRawHandler* handler)
    {
        // All groups...
        bool groupIncluded = groupsModel.all;
        if (groupIncluded)
        {
            return true;
        }

        // Configured groups...
        groupIncluded = groupsModel.configured && handler->IsConfigured();
        if (groupIncluded)
        {
            return true;
        }

        // Listed groups...
        const auto& listedGroups = groupsModel.listedGroups;
        return listedGroups.cend() != listedGroups.find(handler->GetId());
    }

    bool DeviceSchemasHandler::IsTypeIncluded(
        const TypesModel& typesModel,
        const ReportedSchema& schema)
    {
        // All types...
        bool typeInclude = typesModel.all;
        if (typeInclude)
        {
            return true;
        }

        // Listed types...
        const auto& listedTypes = typesModel.listedTypes;
        return listedTypes.cend() != listedTypes.find(schema.GetInterfaceType());
    }

    void DeviceSchemasHandler::BuildReported(
        Json::Value& reportedObject,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        // ToDo: Remove existing, and rebuild list...
        for (const auto& p : (*_rawHandlerMap))
        {
            if (!IsGroupIncluded(_reportingModel.groupsModel, p.second->_rawHandler.get()))
            {
                continue;
            }

            const ReportedSchema& reportedSchema = p.second->_rawHandler->GetReportedSchema();

            if (!IsTypeIncluded(_reportingModel.types, reportedSchema))
            {
                continue;
            }

            if (_reportingModel.tags.listedTags.size() != 0)
            {
                if (!reportedSchema.HasAnyTag(_reportingModel.tags.listedTags))
                {
                    continue;
                }
            }

            reportedObject[p.first] = reportedSchema.ToJsonValue();

            TRACELINEP(LoggingLevel::Verbose, "Id   : ", p.first.c_str());
            TRACELINEP(LoggingLevel::Verbose, "Value: ", reportedObject[p.first].toStyledString().c_str());
        }
    }

    void DeviceSchemasHandler::EmptyReported(
        Json::Value& reportedObject)
    {
        Json::Value nullValue;

        for (const auto& p : (*_rawHandlerMap))
        {
            reportedObject[p.first] = nullValue;
        }
    }

    InvokeResult DeviceSchemasHandler::Invoke(
        const Json::Value& groupDesiredConfigJson) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDesiredState);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        Operation::RunOperation(GetId(), errorList,
            [&]()
        {
            // Make sure this is not a transient state
            if (Operation::IsRefreshing(groupDesiredConfigJson))
            {
                return;
            }

            // Merge...
            // ToDo: Note that this merge causes the loss of which parts are being
            //       set now. For example, if the ntp server was configured earlier
            //       (i.e. cached), and this change doesn't include it, the SetSubGroup()
            //       will still set it again.
            JsonHelpers::Merge(groupDesiredConfigJson, _groupDesiredConfigJson);

            // Processing Meta Data
            _metaData->FromJsonParentObject(groupDesiredConfigJson);

            // Apply new state
            SetSubGroup(_groupDesiredConfigJson);

            // Report current state
            if (_metaData->GetReportingMode() == JsonReportingModeDetailed)
            {
                BuildReported(reportedObject, errorList);
            }
            else
            {
                EmptyReported(reportedObject);
            }
        });

        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }

}}}}
