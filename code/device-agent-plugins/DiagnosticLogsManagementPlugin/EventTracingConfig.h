// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <string>

using namespace DMCommon;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace DiagnosticLogsManagementPlugin {

    class ProviderConfiguration
    {
    public:
        ProviderConfiguration() {};
        ProviderConfiguration( std::string& guid, std::string& traceLevel, std::string& keywords, bool enabled);
        std::string _guid;
        std::string _traceLevel;
        std::string _keywords;
        bool _enabled;

    };

    class CollectorCSPConfiguration
    {
    public:
        CollectorCSPConfiguration() {};
        CollectorCSPConfiguration(std::string& traceLogFileMode, int logFileSizeLimitMB, std::string& logFileFolderName, std::string& logFileName, bool started, std::vector<std::shared_ptr<ProviderConfiguration>> providers);
        std::string _traceLogFileMode;
        int _logFileSizeLimitMB;
        std::string _logFileFolderName;
        std::string _logFileName;
        bool _started;
        std::vector<std::shared_ptr<ProviderConfiguration>> _providers;
    };

    class CollectorConfiguration
    {
    public:
        CollectorConfiguration() 
        {
            this->_subMetaData = std::shared_ptr<MetaData>(new MetaData());
        };
        CollectorConfiguration(std::string& name, std::shared_ptr<MetaData> subMetaData , std::shared_ptr<CollectorCSPConfiguration> cspConfiguration);
        std::string _name;
        std::shared_ptr<MetaData> _subMetaData;
        std::shared_ptr<CollectorCSPConfiguration> _cspConfiguration;
    };

}}}}
