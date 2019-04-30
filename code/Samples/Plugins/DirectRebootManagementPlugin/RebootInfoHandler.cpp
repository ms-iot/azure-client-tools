// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "RebootInfoHandler.h"

#define CSPPath "./Device/Vendor/MSFT/Reboot/Schedule/"

#define CSPSingle "Single"
#define CSPDaily "DailyRecurrent"

using namespace Microsoft::Azure::DeviceManagement::Common;
using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace std;

constexpr char InterfaceVersion[] = "1.0.0";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace RebootManagementPlugin {

    RebootInfoHandler::RebootInfoHandler() :
        MdmHandlerBase(RebootInfoHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, InterfaceVersion))
    {
        // ToDo: This is the last re-start of DM Client - not necessarily the last reboot time.
        _lastRebootTime = Utils::WideToMultibyte(DateTime::GetCurrentDateTimeString().c_str());
    }

    void RebootInfoHandler::SetSubGroup(
        const Json::Value& desiredConfig,
        const string& cspNodeId,
        const string& operationId,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        Operation::RunOperation(operationId, errorList,
            [&]()
        {
            OperationModelT<string> operationModel = Operation::TryGetStringJsonValue(desiredConfig, operationId);

            if (operationModel.present)
            {
                string path = string(CSPPath) + cspNodeId;

                // write
                _mdmProxy.RunSet(path, operationModel.value);

                // Set configured
                _isConfigured = true;
            }
        });
    }

    void RebootInfoHandler::GetSubGroup(
        const string& cspNodeId,
        const string& operationId,
        Json::Value& reportedObject,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        Operation::RunOperation(operationId, errorList,
            [&]()
        {
            string path = string(CSPPath) + cspNodeId;

            // Read back
            string readValue = _mdmProxy.RunGetString(path);
            reportedObject[operationId] = Json::Value(readValue);
        });
    }

    void RebootInfoHandler::Start(
        const Json::Value& config,
        bool& active)
    {
        (void)config;
        active = true;
    }

    void RebootInfoHandler::OnConnectionStatusChanged(
        ConnectionStatus status)
    {
        (void)status;
    }

    void RebootInfoHandler::BuildReported(
        Json::Value& reportedObject,
        std::shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        GetSubGroup(CSPSingle, JsonSingleRebootTime, reportedObject, errorList);
        GetSubGroup(CSPDaily, JsonDailyRebootTime, reportedObject, errorList);

        // The following does not need to be contained in a subgroup.
        reportedObject[JsonLastBootTime] = Json::Value(_lastRebootTime);
    }

    void RebootInfoHandler::EmptyReported(
        Json::Value& reportedObject)
    {
        Json::Value nullValue;
        reportedObject[JsonSingleRebootTime] = nullValue;
        reportedObject[JsonDailyRebootTime] = nullValue;
        reportedObject[JsonLastBootTime] = nullValue;
    }

    InvokeResult RebootInfoHandler::Invoke(
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

            // Processing Meta Data
            _metaData->FromJsonParentObject(groupDesiredConfigJson);
            string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

            //Compare interface version with the interface version sent by service
            if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
            {
                // Apply new state
                SetSubGroup(groupDesiredConfigJson, CSPSingle, JsonSingleRebootTime, errorList);
                SetSubGroup(groupDesiredConfigJson, CSPDaily, JsonDailyRebootTime, errorList);

                // Report current state
                if (_metaData->GetReportingMode() == JsonReportingModeDefault)
                {
                    BuildReported(reportedObject, errorList);
                }
                else
                {
                    EmptyReported(reportedObject);
                }
                _metaData->SetDeviceInterfaceVersion(InterfaceVersion);
            }
            else
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_INTERFACE_VERSION, "Service solution is trying to talk with Interface Version that is not supported.");
            }
        });

        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }

}}}}