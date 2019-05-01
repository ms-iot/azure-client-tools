// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

#include "../../utilities/AutoCloseHandle.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class ServiceParameters
    {
    public:
        ServiceParameters();
        void Load();
        void Save();

        std::string GetLogFilePath() const;
        std::string GetPluginManifestPath() const;
        std::string GetPluginsDataPath() const;
        unsigned int GetSasTokenExpiryInterval() const;         // in seconds
        unsigned int GetSasRenewalGracePeriod() const;          // in seconds
        bool IsDpsEnabled() const;
        std::string GetDpsUri() const;
        std::string GetDpsScopeId() const;
        void WaitDpsScopeId();
        void SetDpsScopeId(const std::string& dpsScopeId);
        unsigned int GetDeviceSlotNumber() const;  // in seconds
        unsigned int GetDmSlotNumber() const;  // in seconds
        std::string GetConnectionString() const;
        const std::map<std::string, unsigned int>& GetOtherModules() const;
        std::string GetDmModuleId() const;
        bool GetHandlerParameters(const std::string& handlerId, Json::Value& handlerParameters) const;
        std::string GetAzureInterfaceType() const;

    private:
        static void ReadJsonString(
            const Json::Value& root,
            const std::string& name,
            const std::string& defaultValue,
            std::string& value);

        static void ReadJsonUInt(
            const Json::Value& root,
            const std::string& name,
            const unsigned int& defaultValue,
            unsigned int& value);

        static void ReadJsonBoolean(
            const Json::Value& root,
            const std::string& name,
            const bool& defaultValue,
            bool& value);

        static void ReadModules(
            const Json::Value& root,
            const std::string& name,
            std::map<std::string, unsigned int>& values);

        static void ReadMap(
            const Json::Value& root,
            const std::string& name,
            std::map<std::string, Json::Value>& configMap);

        void ReadDPSScopeId(const Json::Value& configurations);

        void ReadTPMSlotNumbers(const Json::Value& configurations);

        std::string _textLogFilesPath;
        std::string _pluginManifestsPath;
        std::string _pluginsDataPath;
        unsigned int _sasExpiryInterval;
        unsigned int _sasRenewalGracePeriod;
        bool _dpsIsEnabled;
        std::string _dpsUri;
        std::string _dpsScopeId;
        unsigned int _connectionStringExpiry;
        unsigned int _deviceSlotNumber;
        unsigned int _dmModuleSlotNumber;
        std::string _connectionString;
        std::string _dmModuleId;
        std::string _azureInterfaceType;
        std::map<std::string, unsigned int> _otherModules;
        std::map<std::string, Json::Value> _handlerParametersMap;

        Microsoft::Azure::DeviceManagement::Utils::AutoCloseHandle _validDpsScopeId;
    };

}}}}

