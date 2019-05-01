// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "device-agent/common/DMConstants.h"
#include "device-agent/common/plugins/PluginConstants.h"
#include "DiagnosticLogsManagementStateHandler.h"
#include "EventTracingConfig.h"
#include <iomanip>

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

#define CSPCollectorsRootPath "./Vendor/MSFT/DiagnosticLog/EtwLog/Collectors"
#define CSPQuerySuffix "?list=StructData"
#define CSPNodeType "node"

// CSP Providers
#define CSPTraceLevel "TraceLevel"
#define CSPKeywords "Keywords"
#define CSPState "State"
#define CSPTrue "True"

// CSP Collectors
#define CSPTraceStatus "TraceStatus"
#define CSPTraceLogFileMode "TraceLogFileMode"
#define CSPLogFileSizeLimitMB "LogFileSizeLimitMB"
#define CSPProvidersNode "Providers"
#define CSPTraceControl "TraceControl"
#define CSPTraceControlStart "START"
#define CSPTraceControlStop "STOP"

// Download Channel
#define CSPDataChannel "./Vendor/MSFT/DiagnosticLog/FileDownload/DMChannel"
#define CSPBlockCount "BlockCount"
#define CSPBlockIndexToRead "BlockIndexToRead"
#define CSPBlockData "BlockData"

//registry
#define RegReportToDeviceTwin L"ReportToDeviceTwin"
#define RegEventTracing IoTDeviceAgentRegistryRoot L"\\EventTracingConfiguration"
#define RegEventTracingLogFileFolder L"LogFileFolder"
#define RegEventTracingLogFileName L"LogFileName"

constexpr char InterfaceVersion[] = "1.0.0";

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace DiagnosticLogsManagementPlugin {

    DiagnosticLogsManagementStateHandler::DiagnosticLogsManagementStateHandler() :
        MdmHandlerBase(DiagnosticLogsManagementStateHandlerId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, InterfaceVersion))
    {
    }

    void DiagnosticLogsManagementStateHandler::Start(
        const Json::Value& handlerConfig,
        bool& active)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        SetConfig(handlerConfig);

        Json::Value logFilesPath = handlerConfig[JsonTextLogFilesPath];
        Json::Value dataFilesPath = handlerConfig[JsonPluginsDataPath];
        _dataFolder = dataFilesPath.asString();

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

    void DiagnosticLogsManagementStateHandler::OnConnectionStatusChanged(
        DMCommon::ConnectionStatus status)
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

    vector<shared_ptr<CollectorConfiguration>> DiagnosticLogsManagementStateHandler::GetSubGroup()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
   
        map<string, shared_ptr<CollectorConfiguration>> encounteredCollectorsMap;
        string currentFormattedTime = DateTime::GetFormattedCurrentDateTimeString();
        vector<shared_ptr<CollectorConfiguration>> collectorResponseList;

        std::function<void(std::vector<std::wstring>&, std::wstring&)> valueHandler =
            [&collectorResponseList, &encounteredCollectorsMap, currentFormattedTime](vector<wstring>& wideUriTokens, wstring& wideValue)
        {
            if (wideUriTokens.size() < 7)
            {
                return;
            }
            wstring wideCspCollectorName = wideUriTokens[6];
            string cspCollectorName = WideToMultibyte(wideCspCollectorName.c_str());
            std::shared_ptr<CollectorConfiguration> currentCollector(new CollectorConfiguration());
            currentCollector->_cspConfiguration = std::shared_ptr<CollectorCSPConfiguration>(new CollectorCSPConfiguration());

            // 0/__1___/_2__/______3______/__4___/____5_____/___6___/
            // ./Vendor/MSFT/DiagnosticLog/EtwLog/Collectors/AzureDM
            map<string, shared_ptr<CollectorConfiguration>>::iterator it = encounteredCollectorsMap.find(cspCollectorName);
            if (it == encounteredCollectorsMap.end())
            {
                wstring collectorRegistryPath = RegEventTracing;
                collectorRegistryPath += L"\\";
                collectorRegistryPath += wideCspCollectorName.c_str();

                currentCollector->_name = cspCollectorName;

                currentCollector->_subMetaData->SetReportingMode (WideToMultibyte(Registry::ReadRegistryValue(collectorRegistryPath, RegReportToDeviceTwin, L"detailed" /*default*/).c_str()));
                currentCollector->_cspConfiguration->_logFileFolderName = WideToMultibyte(Registry::ReadRegistryValue(collectorRegistryPath, RegEventTracingLogFileFolder, wideCspCollectorName /*default*/).c_str());
                string defaulEtlFileName = currentCollector->_name + "_" + currentFormattedTime + ".etl";
                currentCollector->_cspConfiguration->_logFileName = WideToMultibyte(Registry::ReadRegistryValue(collectorRegistryPath, RegEventTracingLogFileName, MultibyteToWide(defaulEtlFileName.c_str()) /*default*/).c_str());

                // Add it to the collectors list...
                collectorResponseList.push_back(currentCollector);

                // Save it in the collectors map so that we can find it easily next time 
                // we need it while processing another token...
                encounteredCollectorsMap[cspCollectorName] = currentCollector;
            }
            else
            {
                currentCollector = it->second;
            }
            if (wideUriTokens.size() >= 8)
            {
                wstring wideCspCollectorConfig = wideUriTokens[7];
                string cspCollectorConfig = WideToMultibyte(wideCspCollectorConfig.c_str());

                // 0/__1___/_2__/______3______/__4___/____5_____/___6___/____7______/
                // ./Vendor/MSFT/DiagnosticLog/EtwLog/Collectors/AzureDM/TraceStatus
                if (cspCollectorConfig == CSPTraceStatus)
                {
                    currentCollector->_cspConfiguration->_started = std::stoi(wideValue) == 1 ? true : false;
                }
                else if (cspCollectorConfig == CSPTraceLogFileMode)
                {
                    currentCollector->_cspConfiguration->_traceLogFileMode = (std::stoi(wideValue) == 1 ? JsonFileModeSequential : JsonFileModeCircular);
                }
                else if (cspCollectorConfig == CSPLogFileSizeLimitMB)
                {
                    currentCollector->_cspConfiguration->_logFileSizeLimitMB = std::stoi(wideValue);
                }
            }

            // Is this something under the Providers node?
            if (wideUriTokens.size() >= 9 && WideToMultibyte(wideUriTokens[7].c_str()) == CSPProvidersNode)
            {
                // 0/__1___/_2__/______3______/__4___/____5_____/___6___/____7____/_8__
                // ./Vendor/MSFT/DiagnosticLog/EtwLog/Collectors/AzureDM/Providers/guid
                string providerGuid = WideToMultibyte(wideUriTokens[8].c_str());

                // Do we already have this provider?
                std::shared_ptr<ProviderConfiguration> currentProvider;
                if (!currentCollector->_cspConfiguration->_providers.empty())
                {
                    for (auto provider : currentCollector->_cspConfiguration->_providers)
                    {
                        if (0 == _stricmp(provider->_guid.c_str(), providerGuid.c_str()))
                        {
                            currentProvider = provider;
                        }
                    }
                }

                // If we don't already have the provider, create a new one...
                if (currentProvider == nullptr)
                {
                    std::shared_ptr<ProviderConfiguration> provider(new ProviderConfiguration());
                    provider->_guid = providerGuid;
                    currentCollector->_cspConfiguration->_providers.push_back(provider);
                    currentProvider = provider;
                }

                // Is this a sub-property of the current provider?
                if (wideUriTokens.size() >= 10)
                {
                    // 0/__1___/_2__/______3______/__4___/____5_____/___6___/___7_____/_8__/____9____
                    // ./Vendor/MSFT/DiagnosticLog/EtwLog/Collectors/AzureDM/Providers/guid/TraceLevel

                    wstring wideCspProviderConfig = wideUriTokens[9];
                    string cspProviderConfig = WideToMultibyte(wideCspProviderConfig.c_str());

                    if (cspProviderConfig == CSPTraceLevel)
                    {
                        string jsonValue;
                        if (wideValue == L"1")
                        {
                            jsonValue = JsonTraceLevelCritical;
                        }
                        else if (wideValue == L"2")
                        {
                            jsonValue = JsonTraceLevelError;
                        }
                        else if (wideValue == L"3")
                        {
                            jsonValue = JsonTraceLevelWarning;
                        }
                        else if (wideValue == L"4")
                        {
                            jsonValue = JsonTraceLevelInformation;
                        }
                        else if (wideValue == L"5")
                        {
                            jsonValue = JsonTraceLevelVerbose;
                        }
                        currentProvider->_traceLevel = jsonValue;
                    }
                    else if (cspProviderConfig == CSPKeywords)
                    {
                        currentProvider->_keywords = WideToMultibyte(wideValue.c_str());
                    }
                    else if (cspProviderConfig == CSPState)
                    {
                        currentProvider->_enabled = (0 == _strcmpi(WideToMultibyte(wideValue.c_str()).c_str(), CSPTrue)) ? true : false;
                    }
                }
            }
        };

        string cspQuery;
        cspQuery += CSPCollectorsRootPath;
        cspQuery += CSPQuerySuffix;

        _mdmProxy.RunGetStructData(cspQuery, valueHandler);
        return collectorResponseList;
    }

    void DiagnosticLogsManagementStateHandler::CreateEtlFile(CollectorConfiguration collector)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // The etl file will be created in a folder like this:
        // c:\Data\Users\DefaultAccount\AppData\Local\Temp\IoTDeviceAgent<collector's folder>

        // Construct the data channel...
        string collectorFileCSPPath;
        collectorFileCSPPath += CSPDataChannel;
        collectorFileCSPPath += "/" + collector._name;

        // Read data from CSP into our buffers...
        vector<vector<unsigned char>> decryptedEtlBuffer;
        int blockCount = 0;
        if (_mdmProxy.TryGetNumber(collectorFileCSPPath + "/" + CSPBlockCount, blockCount))
        {
            for (int i = 0; i < blockCount; ++i)
            {
                _mdmProxy.RunSet(collectorFileCSPPath + "/" + CSPBlockIndexToRead, i);
                string blockData = _mdmProxy.RunGetBase64(collectorFileCSPPath + "/" + CSPBlockData);

                vector<unsigned char> decryptedBlock;
                Utils::Base64ToBinary(blockData, decryptedBlock);
                decryptedEtlBuffer.push_back(decryptedBlock);
            }
        }

        // Make sure the target data folder folder exists...
        string etlFolderName;
        etlFolderName = _dataFolder + "\\" + collector._cspConfiguration->_logFileFolderName;
        EnsureFolderExists(etlFolderName);

        // Construct the file name...
        string etlFileName;
        if (collector._cspConfiguration->_logFileName.length() == 0)
        {
            etlFileName = collector._name + "_" + DateTime::GetFormattedCurrentDateTimeString() + ".etl";
        }
        else
        {
            etlFileName = collector._cspConfiguration->_logFileName + ".etl";
        }

        string etlFullFileName = etlFolderName + "\\" + etlFileName;
        TRACELINEP(LoggingLevel::Verbose, "ETL full file name: ", etlFullFileName.c_str());

        // Write the buffers to disk...
        ofstream etlFile(etlFullFileName, ios::out | ios::binary);
        for (auto it = decryptedEtlBuffer.begin(); it != decryptedEtlBuffer.end(); it++)
        {
            etlFile.write(reinterpret_cast<const char *>(it->data()), it->size());
        }
        etlFile.close();
    }

    void DiagnosticLogsManagementStateHandler::ApplyCollectorConfiguration(CollectorConfiguration collector,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        Operation::RunOperation(GetId(), errorList,
            [&]()
        {
            // Build paths...
            string collectorCSPPath = CSPCollectorsRootPath;
            collectorCSPPath += "/" + collector._name;

            wstring collectorRegistryPath = RegEventTracing;
            collectorRegistryPath += L"\\";
            collectorRegistryPath += MultibyteToWide(collector._name.c_str());

            Registry::WriteRegistryValue(collectorRegistryPath, RegReportToDeviceTwin, MultibyteToWide(collector._subMetaData->GetReportingMode().c_str()));

            if(collector._cspConfiguration)
            {
                if (collector._cspConfiguration->_logFileFolderName.length() == 0)
                {
                    TRACELINE(LoggingLevel::Verbose, "Warning: LogFileFolder is empty. Using collector name as the LogFileFolder.");
                    collector._cspConfiguration->_logFileFolderName = collector._name;
                }

                // Make sure the user is not trying to access any files outside the IoTDM folder.
                if (nullptr != strstr(collector._cspConfiguration->_logFileFolderName.data(), "..") ||
                    nullptr != strstr(collector._cspConfiguration->_logFileFolderName.data(), "\\") ||
                    nullptr != strstr(collector._cspConfiguration->_logFileFolderName.data(), "/"))
                {
                    TRACELINE(LoggingLevel::Verbose, "Error: LogFileFolder cannot contain '/', '\\', or '..'.");//remove later
                    throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "LogFileFolder cannot contain '/', '\\', or '..'.");
                }

                if (collector._cspConfiguration->_logFileName.length() != 0)
                {
                    if (nullptr != strstr(collector._cspConfiguration->_logFileName.data(), "..") ||
                        nullptr != strstr(collector._cspConfiguration->_logFileName.data(), "\\") ||
                        nullptr != strstr(collector._cspConfiguration->_logFileName.data(), "/"))
                    {
                        TRACELINE(LoggingLevel::Verbose, "Error: LogFileName cannot contain '/', '\\', or '..'.");//remove later
                        throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "LogFileName cannot contain '/', '\\', or '..'.");
                    }
                }

                const string providersCSPPath = collectorCSPPath + "/" + CSPProvidersNode;

                // Capture if collector is already part of this CSP configuration...
                string collectorString = _mdmProxy.RunGetString(CSPCollectorsRootPath);
                // Is the collector already part of this CSP configuration?
                if (string::npos == collectorString.find(collector._name))
                {
                    _mdmProxy.RunAdd(CSPCollectorsRootPath, collector._name);;
                }

                Registry::WriteRegistryValue(collectorRegistryPath, RegEventTracingLogFileFolder, MultibyteToWide(collector._cspConfiguration->_logFileFolderName.c_str()));
                Registry::WriteRegistryValue(collectorRegistryPath, RegEventTracingLogFileName, MultibyteToWide(collector._cspConfiguration->_logFileName.c_str()));
                _mdmProxy.RunSet(collectorCSPPath + "/" + CSPLogFileSizeLimitMB, collector._cspConfiguration->_logFileSizeLimitMB);
                _mdmProxy.RunSet(collectorCSPPath + "/" + CSPTraceLogFileMode, collector._cspConfiguration->_traceLogFileMode == JsonFileModeSequential ? 1 : 2);

                // Capture which providers are already part of this CSP collector configuration...
                string providersString = _mdmProxy.RunGetString(providersCSPPath);

                // Iterate though each desired provider and add/apply its settings...
                for (shared_ptr<ProviderConfiguration> provider : collector._cspConfiguration->_providers)
                {
                    string providerCSPPath = collectorCSPPath + "/" + CSPProvidersNode + "/" + provider->_guid;

                    // Is the provider already part of this CSP collector configuration?
                    if (string::npos == providersString.find(provider->_guid))
                    {
                        _mdmProxy.RunAddTyped(providerCSPPath, CSPNodeType);
                    }

                    int traceLevel = 0;
                    if (0 == _stricmp(provider->_traceLevel.c_str(), JsonTraceLevelCritical))
                    {
                        traceLevel = 1;
                    }
                    else if (0 == _stricmp(provider->_traceLevel.c_str(), JsonTraceLevelError))
                    {
                        traceLevel = 2;
                    }
                    else if (0 == _stricmp(provider->_traceLevel.c_str(), JsonTraceLevelWarning))
                    {
                        traceLevel = 3;
                    }
                    else if (0 == _stricmp(provider->_traceLevel.c_str(), JsonTraceLevelInformation))
                    {
                        traceLevel = 4;
                    }
                    else if (0 == _stricmp(provider->_traceLevel.c_str(), JsonTraceLevelVerbose))
                    {
                        traceLevel = 5;
                    }

                    _mdmProxy.RunSet(providerCSPPath + "/" + CSPState, provider->_enabled);
                    _mdmProxy.RunSet(providerCSPPath + "/" + CSPKeywords, provider->_keywords);
                    _mdmProxy.RunSet(providerCSPPath + "/" + CSPTraceLevel, traceLevel);
                }

                // Finally process the started/stopped status...
                unsigned int traceStatus = _mdmProxy.RunGetUInt(collectorCSPPath + "/" + CSPTraceStatus);
                if (collector._cspConfiguration->_started)
                {
                    if (traceStatus == 0 /*stopped*/)
                    {
                        TRACELINEP(LoggingLevel::Verbose, "Starting logging for : ", collector._name.c_str());
                        _mdmProxy.RunExecWithParameters(collectorCSPPath + "/" + CSPTraceControl, CSPTraceControlStart);
                    }
                }
                else
                {
                    if (traceStatus == 1 /*started*/)
                    {
                        TRACELINEP(LoggingLevel::Verbose, "Stopping logging for : ", collector._name.c_str());
                        _mdmProxy.RunExecWithParameters(collectorCSPPath + "/" + CSPTraceControl, CSPTraceControlStop);

                        CreateEtlFile(collector);
                    }
                }
            }
        });
    }

    void DiagnosticLogsManagementStateHandler::SetSubGroup(
        const Json::Value& desiredConfig,
        shared_ptr<DMCommon::ReportedErrorList> errorList) {

        // Get required parameters first...
        vector<CollectorConfiguration> collectorConfigurations;
        for (Json::Value::const_iterator collector = desiredConfig.begin(); collector != desiredConfig.end(); collector++)
        {
            Operation::RunOperation(GetId(), errorList,
                [&]()
            {
                CollectorConfiguration collectorDesiredConfiguration;
                vector<shared_ptr<ProviderConfiguration>> providersConfigList;

                string collectorName = collector.key().asString();

                std::shared_ptr<MetaData> _subMetaData(new MetaData());
                _subMetaData->FromJsonParentObjectSubMeta(*collector);

                shared_ptr<CollectorCSPConfiguration> collectorCSPConfig;
                unsigned int fieldCount = 0;

                OperationModelT<string> traceLogFileMode = Operation::TryGetStringJsonValue(*collector, JsonCollectorTraceLogFileMode); //optional property
                OperationModelT<string>  logFileFolderName = Operation::TryGetStringJsonValue(*collector, JsonCollectorLogFileFolder);
                OperationModelT<string> logFileName = Operation::TryGetStringJsonValue(*collector, JsonCollectorLogFileName);
                OperationModelT<int> logFileSizeLimitMB = Operation::TryGetIntJsonValue(*collector, JsonCollectorLogFileSizeLimitMB); // optional property
                OperationModelT<bool> started = Operation::TryGetBoolJsonValue(*collector, JsonCollectorStarted);
                OperationModel providers = Operation::TryGetJsonValue(*collector, JsonProviders);
                
                fieldCount += logFileFolderName.present ? 1 : 0;
                fieldCount += logFileName.present ? 1 : 0;
                fieldCount += started.present ? 1 : 0;
                fieldCount += providers.present ? 1 : 0;
                
                //only all or nothing are allowed
                if (fieldCount != 4 and fieldCount != 0)
                {
                    throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "Missing collector fields");
                }

                // If nothing, return...
                if (fieldCount == 0)
                {
                    TRACELINE(LoggingLevel::Verbose, "No collector fields are defined. Returning.");

                }
                else
                {
                    //get the provider config for the collector
                    for (Json::Value::const_iterator provider = providers.value.begin(); provider != providers.value.end(); provider++)
                    {
                        string guid = provider.key().asString();
                        OperationModelT<bool> enabled = Operation::TryGetBoolJsonValue(*provider, JsonProviderEnabled);
                        OperationModelT<string> traceLevel = Operation::TryGetStringJsonValue(*provider, JsonProviderTraceLevel);
                        OperationModelT<string> keywords = Operation::TryGetStringJsonValue(*provider, JsonProviderKeywords); // optional parameter

                        if (!enabled.present || !traceLevel.present)
                        {
                            throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_INVALID_JSON_FORMAT, "Missing provider properties in JSON");
                        }

                        shared_ptr<ProviderConfiguration> providerConfig(new ProviderConfiguration(guid, traceLevel.value, keywords.value, enabled.value));
                        providersConfigList.push_back(providerConfig);
                    }
                    collectorCSPConfig = shared_ptr<CollectorCSPConfiguration>(new CollectorCSPConfiguration(traceLogFileMode.value, logFileSizeLimitMB.value, logFileFolderName.value, logFileName.value, started.value, providersConfigList));
                }

                collectorDesiredConfiguration = CollectorConfiguration(collectorName, _subMetaData, collectorCSPConfig);
                collectorConfigurations.push_back(collectorDesiredConfiguration);
            });
        }

        for (CollectorConfiguration collector : collectorConfigurations)
        {
            ApplyCollectorConfiguration(collector,errorList);
        }
    }

    void DiagnosticLogsManagementStateHandler::BuildReported(
        Json::Value& reportedObject,
        shared_ptr<DMCommon::ReportedErrorList> errorList)
    {
        vector<shared_ptr<CollectorConfiguration>> collectorResponseList;
        collectorResponseList = GetSubGroup();

        Json::Value nullValue;
        Json::Value reportedCollectorList = nullValue; //set the event tracing reported to a null value initially

        for (auto collector : collectorResponseList)
        {
            string applicableReporting = collector->_subMetaData->GetReportingMode();
            if (applicableReporting == JsonReportLevelDetailed)
            {
                Json::Value reportedCollector;
                reportedCollector[JsonCollectorTraceLogFileMode] = Json::Value(collector->_cspConfiguration->_traceLogFileMode);
                reportedCollector[JsonCollectorLogFileSizeLimitMB] = Json::Value(collector->_cspConfiguration->_logFileSizeLimitMB);
                reportedCollector[JsonCollectorLogFileFolder] = Json::Value(collector->_cspConfiguration->_logFileFolderName);
                reportedCollector[JsonCollectorLogFileName] = Json::Value(collector->_cspConfiguration->_logFileName);
                reportedCollector[JsonCollectorStarted] = Json::Value(collector->_cspConfiguration->_started);
                if (!collector->_cspConfiguration->_providers.empty())
                {
                    Json::Value reportedProvider;
                    for (auto provider : collector->_cspConfiguration->_providers)
                    {
                        Json::Value reportedProviderConfig;
                        reportedProviderConfig[JsonProviderEnabled] = Json::Value(provider->_enabled);
                        reportedProviderConfig[JsonProviderKeywords] = Json::Value(provider->_keywords);
                        reportedProviderConfig[JsonProviderTraceLevel] = Json::Value(provider->_traceLevel);
                        reportedProvider[provider->_guid] = reportedProviderConfig;
                    }
                    reportedCollector[JsonProviders] = reportedProvider;
                }
                reportedCollectorList[collector->_name] = Json::Value(reportedCollector);
            }
            else if (applicableReporting == JsonReportLevelMinimal)
            {
                string emptyString = "";
                reportedCollectorList[collector->_name] = Json::Value(emptyString.c_str());
            }
        }
        reportedObject = reportedCollectorList;
    }

    InvokeResult DiagnosticLogsManagementStateHandler::Invoke(
        const Json::Value& groupDesiredConfigJson) noexcept
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Returned objects (if InvokeContext::eDesiredState, no return result present for invokeResult).
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
            // Process Meta Data
            _metaData->FromJsonParentObject(groupDesiredConfigJson);
            string serviceInterfaceVersion = _metaData->GetServiceInterfaceVersion();

            // Report refreshing
            ReportRefreshing();

            //Compare interface version with the interface version sent by service
            if (MajorVersionCompare(InterfaceVersion, serviceInterfaceVersion) == 0)
            {
                SetSubGroup(groupDesiredConfigJson, errorList);

                BuildReported(reportedObject, errorList);
                _metaData->SetDeviceInterfaceVersion(InterfaceVersion);
            }
            else
            {
                throw DMException(DMSubsystem::DeviceAgentPlugin, DM_PLUGIN_ERROR_INVALID_INTERFACE_VERSION, "Service solution is trying to talk with Interface Version that is not supported.");
            }
        });

        // Update device twin
        FinalizeAndReport(reportedObject, errorList);

        return invokeResult;
    }

}}}}