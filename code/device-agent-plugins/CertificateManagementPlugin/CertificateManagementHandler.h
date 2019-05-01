// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <list>
#include "device-agent/common/MdmHandlerBase.h"
#include "CertificateFile.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace CertificateManagementPlugin {

    class CertificateManagementHandler : public DMCommon::MdmHandlerBase
    {
    public:
        CertificateManagementHandler();

        void Start(
            const Json::Value& config,
            bool& active);

        void OnConnectionStatusChanged(
            DMCommon::ConnectionStatus status);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredConfig) noexcept;
    private:

        std::string DownloadCertificateFromBlob(
            const std::string& connectionString,
            const std::string& containerAndBlob,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void UninstallCertificateHandler(
            const std::string& cspPath,
            const std::string& hash,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        bool InstallCertificateHandler(
            const std::string& cspPath,
            const std::string& hash,
            const std::string& certificateInBase64,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        Json::Value CreateInstalledCertificateJsonList(
            std::vector<std::string>& installedCertificates);

        void ModifyCertificatesHandler(
            const std::string& operationId,
            const std::string& cspPath,
            const Json::Value& desiredConfig,
            const DMCommon::OperationModelT<std::string>& connectionString,
            std::map<std::string, std::vector<std::string>>& installedCertificates,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void BuildReported(
            Json::Value& reportedObject,
            std::map<std::string, std::vector<std::string>>& installedCertificates,
            std::shared_ptr<DMCommon::ReportedErrorList> errorList);

        void EmptyReported(
            Json::Value& reportedObject);
    };

}}}}
