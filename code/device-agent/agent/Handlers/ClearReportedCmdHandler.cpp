// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <assert.h>
#include "ClearReportedCmdHandler.h"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    ClearReportedCmdHandler::ClearReportedCmdHandler(
        const std::function<void(std::vector<std::string>& handlerIds)>& getHandlerIds,
        const std::function<void(const Json::Value& reportedProperties)>& reportAll) :
        HandlerBase(JsonClearReportedCmd, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1)),
        GetHandlerIds(getHandlerIds),
        ReportAll(reportAll)
    {
    }

    void ClearReportedCmdHandler::Start(
        const Json::Value& handlerConfig,
        bool& active)
    {
        SetConfig(handlerConfig);
        active = true;
    }

    void ClearReportedCmdHandler::OnConnectionStatusChanged(
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
        }
    }

    InvokeResult ClearReportedCmdHandler::Invoke(
        const Json::Value& jsonParameters) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDirectMethod, JsonDirectMethodSuccessCode, JsonDirectMethodEmptyPayload);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        Operation::RunOperation(GetId(), errorList,
            [&]()
        {
            // Process Meta Data
            _metaData->FromJsonParentObject(jsonParameters);

            vector<string> handlerIds;
            if (GetHandlerIds)
            {
                GetHandlerIds(handlerIds);
            }

            Json::Value emptyValue;
            Json::Value reportedProperties(Json::objectValue);
            for (const string& handlerId : handlerIds)
            {
                if (handlerId == GetId())
                {
                    continue;
                }
                reportedProperties[handlerId.c_str()] = emptyValue;
            }

            if (ReportAll)
            {
                ReportAll(reportedProperties);
            }
        });

        FinalizeAndReport(reportedObject, errorList);

        // Pack return payload
        if (errorList->Count() != 0)
        {
            invokeResult.code = JsonDirectMethodFailureCode;
            invokeResult.payload = errorList->ToJsonObject()[GetId()].toStyledString();
        }
        return invokeResult;
    }

}}}}
