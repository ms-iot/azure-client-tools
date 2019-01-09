// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CertificateDetailedInfoHandler.h"
#include "CertificateFile.h"
#include "PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\DMConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"

using namespace DMUtils;
using namespace DMCommon;
using namespace std;

const string certInfoIssuedBy = "/IssuedBy";
const string certInfoIssuedTo = "/IssuedTo";
const string certInfoValidFrom = "/ValidFrom";
const string certInfoValidTo = "/ValidTo";
const string certEncodedCertificate = "/EncodedCertificate";
const string certInfoTemplateName = "/TemplateName";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace CertificateManagementPlugin {

    CertificateDetailedInfoHandler::CertificateDetailedInfoHandler() :
        BaseHandler(CertificateDetailedInfoHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
    {
    }

    void CertificateDetailedInfoHandler::GetCertificateDetailHandler(
        const Json::Value& desiredConfig,
        Json::Value& reportedObject,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        RunOperation("certDetail", errorList,
            [&]()
        {
            string cspPath = GetSinglePropertyOpStringParameter(desiredConfig, "path");
            string hash = GetSinglePropertyOpStringParameter(desiredConfig, "hash");

            string certPath = cspPath + "/" + hash;
            // Merge...
            // n/a because this operation is a single-field operation.

            // Parse...
            string issuedBy = _mdmProxy.RunGetString(certPath + certInfoIssuedBy);
            reportedObject[JsonCertificateIssuedBy] = Json::Value(issuedBy.c_str());
            string issuedTo = _mdmProxy.RunGetString(certPath + certInfoIssuedTo);
            reportedObject[JsonCertificateIssuedTo] = Json::Value(issuedTo.c_str());
            string validFrom = _mdmProxy.RunGetString(certPath + certInfoValidFrom);
            reportedObject[JsonCertificateValidFrom] = Json::Value(validFrom.c_str());
            string validTo = _mdmProxy.RunGetString(certPath + certInfoValidTo);
            reportedObject[JsonCertificateValidTo] = Json::Value(validTo.c_str());
            string base64Encoding = _mdmProxy.RunGetString(certPath + certEncodedCertificate);
            reportedObject[JsonCertificateBase64Encoding] = Json::Value(base64Encoding.c_str());
            string templateName = _mdmProxy.RunGetString(certPath + certInfoTemplateName);
            reportedObject[JsonCertificateTemplateName] = Json::Value(templateName.c_str());
        });
    }

    void CertificateDetailedInfoHandler::Start(
        const Json::Value& handlerConfig,
        bool& active)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        SetConfig(handlerConfig);

        // Text file logging...
        Json::Value logFilesPath = handlerConfig[JsonTextLogFilesPath];
        if (!logFilesPath.isNull() && logFilesPath.isString())
        {
            wstring wideLogFileName = MultibyteToWide(logFilesPath.asString().c_str());
            wstring wideLogFileNamePrefix = MultibyteToWide(CertificateDetailedInfoHandlerId);
            gLogger.SetLogFilePath(wideLogFileName.c_str(), wideLogFileNamePrefix.c_str());
            gLogger.EnableConsole(true);

            TRACELINE(LoggingLevel::Verbose, "Logging configured.");
        }

        active = true;
    }

    void CertificateDetailedInfoHandler::OnConnectionStatusChanged(
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

    InvokeResult CertificateDetailedInfoHandler::Invoke(
        const Json::Value& jsonParameters) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDirectMethod, it is returned to the cloud direct method caller).
        InvokeResult invokeResult(InvokeContext::eDirectMethod, JsonDirectMethodSuccessCode, JsonDirectMethodEmptyPayload);

        // Twin reported objects
        Json::Value reportedObject(Json::objectValue);
        std::shared_ptr<ReportedErrorList> errorList = make_shared<ReportedErrorList>();

        RunOperation(GetId(), errorList,
            [&]()
        {
            // Process Meta Data
            _metaData->FromJsonParentObject(jsonParameters);

            // Apply
            GetCertificateDetailHandler(jsonParameters, reportedObject, errorList);
        });

        // Update device twin
        // This direct method doesn't update the twin.

        // Pack return payload
        if (errorList->Count() == 0)
        {
            invokeResult.payload = reportedObject.toStyledString();
        }
        else
        {
            invokeResult.code = JsonDirectMethodFailureCode;
            invokeResult.payload = errorList->ToJsonObject()[GetId()].toStyledString();
        }
        return invokeResult;
    }

}}}}
