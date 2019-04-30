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

        static OperationModelT<int> TryGetIntJsonValue(
            const Json::Value& groupRoot,
            const std::string& propertyName);

        static OperationModelT<bool> TryGetBoolJsonValue(
            const Json::Value& groupRoot,
            const std::string& propertyName);

        static OperationModelT<std::string> TryGetStringJsonValue(
            const Json::Value& groupRoot,
            const std::string& propertyName);

        static std::string GetStringJsonValue(
            const Json::Value& groupRoot,
            const std::string& propertyName);

        static bool IsRefreshing(
            const Json::Value& desiredConfig);
    };

}}}}
