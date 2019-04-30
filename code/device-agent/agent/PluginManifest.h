// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class HandlerConfiguration
    {
    public:
        HandlerConfiguration(const Json::Value&);

        std::string GetId() const;

        std::vector<std::string> GetDependencies() const;

    private:

        std::string _id;
        std::vector<std::string> _dependencies;
      };

    class PluginManifest
    {
    public:

        PluginManifest();

        void FromJsonFile(const std::string& manifestFileName);

        std::string GetCodeFileName() const;
        bool IsDirect() const;
        bool IsOutOfProc() const;
        long GetKeepAliveTime() const;
        std::string GetAgentPluginProtocolVersion() const;
        const std::map<std::string, std::shared_ptr<HandlerConfiguration>>& GetHandlers() const;

    private:

        std::string _codeFileName;
        bool _isDirect;
        bool _isOutOfProc;
        long _keepAliveTime;
        std::string _agentPluginProtocolVersion;
        std::map<std::string, std::shared_ptr<HandlerConfiguration>> _handlerInfoList;
    };

}}}}