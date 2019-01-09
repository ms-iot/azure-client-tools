// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "..\IoThub\IoTHubInterfaces.h"
#include "..\AzureRawHost.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class DeviceSchemasHandler : public DMCommon::BaseHandler
    {
        struct GroupsModel
        {
            bool all;
            bool configured;
            std::set<std::string> listedGroups;

            GroupsModel() :
                all(false),
                configured(false)
            {}

            static GroupsModel FromJson(const Json::Value& parent);
        };

        struct TypesModel
        {
            bool all;
            std::set<std::string> listedTypes;

            TypesModel() :
                all(false)
            {}

            static TypesModel FromJson(const Json::Value& parent);
        };

        struct TagsModel
        {
            std::set<std::string> listedTags;

            static TagsModel FromJson(const Json::Value& parent);
        };

        struct ReportingModel
        {
            GroupsModel groupsModel;
            TypesModel types;
            TagsModel tags;

            static ReportingModel FromJson(const Json::Value& parent);
        };

    public:
        DeviceSchemasHandler(
            const RawHandlerMapType* configurationHandlerMap);

        // IRawHandler
        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredConfig) noexcept;

    private:

        void SetSubGroup(
            const Json::Value& groupRoot);

        static bool IsGroupIncluded(
            const GroupsModel& groupsModel,
            const IRawHandler* handler);

        static bool IsTypeIncluded(
            const TypesModel& typesModel,
            const Microsoft::Azure::DeviceManagement::Common::ReportedSchema& schema);

        void BuildReported(
            Json::Value& reportedObject,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void EmptyReported(
            Json::Value& reportedObject);

        // Date Members
        Json::Value _groupDesiredConfigJson;
        ReportingModel _reportingModel;
        const RawHandlerMapType* _rawHandlerMap;
    };

}}}}
