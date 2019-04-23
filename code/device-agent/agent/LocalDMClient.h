// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include "ServiceParameters.h"
#include "DeviceClientInterfaces.h"
#include "LocalDMClient.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class LocalDMClient : public DMCommon::ILocalDMClient
    {
    public:
        LocalDMClient(
            std::shared_ptr<ServiceParameters> serviceParameters,
            std::shared_ptr<IRawHandlerRouter> rawHandlerRouter);

        void InvokeReboot();
        void SetScopeId(const std::wstring& scopeId);
        void GetRegistrationId(std::wstring& registrationId);
        void GetEndorsementKey(std::wstring& endorsementKey);
        void GetConnetionString(int slot, long long expiry, std::wstring& connectionString);

    private:
        std::shared_ptr<ServiceParameters> _serviceParameters;
        std::shared_ptr<IRawHandlerRouter> _rawHandlerRouter;
    };

}}}}