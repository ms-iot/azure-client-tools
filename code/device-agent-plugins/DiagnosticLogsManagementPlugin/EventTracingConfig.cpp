// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "EventTracingConfig.h"
#include <iomanip>
#include <Wincrypt.h>

using namespace DMCommon;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace DiagnosticLogsManagementPlugin {

    ProviderConfiguration::ProviderConfiguration(std::string& guid, std::string& traceLevel, std::string& keywords, bool enabled)
    {
        this->_guid = guid;
        this->_enabled = enabled;
        this->_keywords = keywords;
        this->_traceLevel = traceLevel;
    }

    CollectorCSPConfiguration::CollectorCSPConfiguration(std::string& traceLogFileMode, int logFileSizeLimitMB, std::string& logFileFolderName, std::string& logFileName, bool started, std::vector<std::shared_ptr<ProviderConfiguration>> providers)
    {
        this->_traceLogFileMode = traceLogFileMode;
        this->_logFileSizeLimitMB = logFileSizeLimitMB;
        this->_logFileFolderName = logFileFolderName;
        this->_logFileName = logFileName;
        this->_started = started;
        this->_providers = providers;
    }

    CollectorConfiguration::CollectorConfiguration(std::string& name, std::shared_ptr<MetaData> subMetaData, std::shared_ptr<CollectorCSPConfiguration> cspConfiguration)
    {
        this->_name = name;
        this->_subMetaData = subMetaData;
        this->_cspConfiguration = cspConfiguration;
    }

}}}}
