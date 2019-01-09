// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\DMConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "SkeletonCmdHandler.h"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Windows { namespace Azure { namespace DeviceManagement { namespace SkeletonPlugin {

    SkeletonCmdHandler::SkeletonCmdHandler() :
        BaseHandler(SkeletonCmdHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
    {
    }

    void SkeletonCmdHandler::Start(
        const Json::Value& config,
        bool& active)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Json::Value logFilesPath = config[JsonTextLogFilesPath];
        if (!logFilesPath.isNull() && logFilesPath.isString())
        {
            wstring wideLogFileName = MultibyteToWide(logFilesPath.asString().c_str());
            wstring wideLogFileNamePrefix = MultibyteToWide(GetId().c_str());
            gLogger.SetLogFilePath(wideLogFileName.c_str(), wideLogFileNamePrefix.c_str());
            gLogger.EnableConsole(true);

            TRACELINE(LoggingLevel::Verbose, "Logging configured.");
        }

        active = true;
    }

    InvokeResult SkeletonCmdHandler::Invoke(
        const Json::Value& desiredConfig) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        (void)desiredConfig;

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDirectMethod, JsonDirectMethodSuccessCode, JsonDirectMethodEmptyPayload);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        RunOperation(GetId(), errorList,
            [&]()
        {
            // Get required parameters first...
            // string param0 = GetSinglePropertyOpStringParameter(desiredConfig, "param0");

            // Get optional parameters...
            // OperationModelT<bool> param1 = TryGetOptionalSinglePropertyOpBoolParameter(desiredConfig, "param1");

            // Start processing...
            // Do the actual work here.

            // if (returnCode != 0)
            // {
            //    throw DMException(returnCode, "Error: ApplyUpdate.exe returned an error code.");
            // }
        });

        // Update device twin
        FinalizeAndReport(reportedObject, errorList);

        // Pack return payload
        if (errorList->Count() != 0)
        {
            invokeResult.code = JsonDirectMethodFailureCode;
            invokeResult.payload = errorList->ToJsonObject()[GetId()].toStyledString();
        }
        return invokeResult;
    }

}}}}}
