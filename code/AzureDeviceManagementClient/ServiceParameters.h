// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class ServiceParameters
    {
    public:
        ServiceParameters();
        void Load();

        std::string GetLogFilePath() const;
        std::string GetPluginManifestPath() const;
        unsigned int GetSasTokenExpiryInterval() const;         // in seconds
        unsigned int GetSasRenewalGracePeriod() const;          // in seconds
        std::string GetDpsUri() const;
        std::string GetDpsScopeId() const;
        unsigned int GetDeviceSlotNumber() const;  // in seconds
        unsigned int GetDmSlotNumber() const;  // in seconds
        std::string GetDebugConnectionString() const;
        const std::map<std::string, unsigned int>& GetOtherModules() const;
        std::string GetDmModuleId() const;
        bool GetHandlerParameters(const std::string& handlerId, Json::Value& handlerParameters) const;

    private:
        static void ReadJsonString(
            const Json::Value& root,
            const std::string& name,
            const std::string& defaultValue,
            std::string& value);

        static void ReadModules(
            const Json::Value& root,
            const std::string& name,
            std::map<std::string, unsigned int>& values);
            
        static void ReadJsonUInt(
            const Json::Value& root,
            const std::string& name,
            const unsigned int& defaultValue,
            unsigned int& value);

        static void ReadMap(
            const Json::Value& root,
            const std::string& name,
            std::map<std::string, Json::Value>& configMap);

        void ReadDPSScopeId(const Json::Value& configurations);

        void ReadTPMSlotNumbers(const Json::Value& configurations);

        std::string _textLogFilesPath;
        std::string _pluginManifestsPath;
        unsigned int _sasExpiryInterval;
        unsigned int _sasRenewalGracePeriod;
        std::string _dpsUri;
        std::string _dpsScopeId;
        unsigned int _connectionStringExpiry;
        unsigned int _deviceSlotNumber;
        unsigned int _dmModuleSlotNumber;
        std::string _debugConnectionString;
        std::string _dmModuleId;
        std::map<std::string, unsigned int> _otherModules;
        std::map<std::string, Json::Value> _handlerParametersMap;
    };

}}}}
