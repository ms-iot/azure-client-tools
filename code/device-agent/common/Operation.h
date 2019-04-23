// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReportedErrorList.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    struct OperationModel
    {
        bool present;
        Json::Value value;

        OperationModel() :
            present(false)
        {}
    };

    template<class T>
    struct OperationModelT
    {
        bool present;
        T value;

        OperationModelT() :
            present(false)
        {}
    };

    class Operation
    {
    public:
        static bool RunOperation(
            const std::string& operationId,
            std::shared_ptr<ReportedErrorList> errorList,
            const std::function<void()>& Action);

        // ToDo: Better name/better params.

        static OperationModel TryGetJsonValue(
            const Json::Value& jsonObject,
            const std::string& propertyName);

        static OperationModelT<int> TryGetOptionalSinglePropertyOpIntParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        static OperationModelT<bool> TryGetOptionalSinglePropertyOpBoolParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        static OperationModelT<std::string> TryGetOptionalSinglePropertyOpStringParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        static std::string GetSinglePropertyOpStringParameter(
            const Json::Value& groupRoot,
            const std::string& operationId);

        static bool IsRefreshing(
            const Json::Value& desiredConfig);
    };

}}}}
