// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ServiceParameters.h"
#include "DMConstants.h"

using namespace DMUtils;
using namespace std;
using namespace std::experimental::filesystem;
using namespace std::experimental::filesystem::v1;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

ServiceParameters::ServiceParameters() :
    _textLogFilesPath(LogFilePathDefaultValue),
    _pluginManifestsPath(PluginManifestPathDefaultValue),
    _sasExpiryInterval(SasExpiryIntervalDefaultValue),
    _sasRenewalGracePeriod(SasRenewalGracePeriodDefaultValue),
    _dpsUri(DpsUriDefaultValue),
    _dpsScopeId(DpsScopeIdDefaultValue),
    _deviceSlotNumber(DeviceSlotNumberDefaultValue),
    _dmModuleSlotNumber(DmModuleSlotNumberDefaultValue),
    _debugConnectionString(DefaultDebugConnectionString),
    _dmModuleId(DefaultModuleId)
{
}

void ServiceParameters::ReadJsonString(
    const Json::Value& root,
    const std::string& name,
    const std::string& defaultValue,
    std::string& value)
{
    Json::Value jsonValue = root[name];
    if (jsonValue.isNull() || !jsonValue.isString())
    {
        value = defaultValue;
    }

    value = jsonValue.asString();
}

void ServiceParameters::ReadModules(
    const Json::Value& root,
    const std::string& name,
    std::map<std::string, unsigned int>& values)
{
    values.clear();

    Json::Value jsonValue = root[name];
    if (jsonValue.isNull() || !jsonValue.isObject())
    {
        return;
    }

    vector<string> keys = jsonValue.getMemberNames();
    for (const string& key : keys)
    {
        values[key] = jsonValue[key].asUInt();
    }
}

void ServiceParameters::ReadJsonUInt(
    const Json::Value& root,
    const std::string& name,
    const unsigned int& defaultValue,
    unsigned int& value)
{
    Json::Value jsonValue = root[name];
    if (jsonValue.isNull() || !jsonValue.isUInt())
    {
        value = defaultValue;
    }

    value = jsonValue.asUInt();
}

void ServiceParameters::ReadMap(
    const Json::Value& root,
    const std::string& name,
    std::map<std::string, Json::Value>& configMap)
{
    configMap.clear();

    Json::Value jsonValue = root[name];
    if (jsonValue.isNull() || !jsonValue.isObject())
    {
        return;
    }

    vector<string> keys = jsonValue.getMemberNames();
    for (const string& key : keys)
    {
        configMap[key] = jsonValue[key];
    }
}

void ServiceParameters::ReadDPSScopeId(const Json::Value& configurations)
{
    bool WDPEnabled = false;

#if defined(_DEBUG)
    wstring registryIdScope;
    LSTATUS result = Registry::TryReadRegistryValue(RootRegistryKey, WDPRegisteryScopeId, registryIdScope);
    if (result == ERROR_SUCCESS)
    {
        // Configured through WDP.
        _dpsScopeId = Utils::WideToMultibyte(registryIdScope.c_str());
        WDPEnabled = true;
    }
#endif

    if (!WDPEnabled)
    {
        ReadJsonString(configurations, JsonDpsScopeId, DpsScopeIdDefaultValue, _dpsScopeId);
    }
}

void ServiceParameters::ReadTPMSlotNumbers(const Json::Value& configurations)
{
    bool WDPEnabled = false;

#if defined(_DEBUG)
    unsigned long registrySlotId = 0;
    LSTATUS result = Registry::TryReadRegistryValue(RootRegistryKey, WDPRegisteryLogicalDeviceId, registrySlotId);
    if (result == ERROR_SUCCESS)
    {
        _deviceSlotNumber = registrySlotId;
        _dmModuleSlotNumber = registrySlotId;
        WDPEnabled = true;
    }
#endif

    if (!WDPEnabled)
    {
        ReadJsonUInt(configurations, JsonDeviceSlotNumber, DeviceSlotNumberDefaultValue, _deviceSlotNumber);
        ReadJsonUInt(configurations, JsonDmModuleSlotNumber, DmModuleSlotNumberDefaultValue, _dmModuleSlotNumber);
    }
}

void ServiceParameters::Load()
{
    string rootPath = GetProcessPath();
    string configurationFilePath = rootPath + "\\" + ServiceConfigurationFile;

    Json::Value configurations;
    if (!JsonUtils::TryParseJSONFile(configurationFilePath, configurations))
    {
        return;
    }

    ReadJsonString(configurations, JsonTextLogFilesPath, LogFilePathDefaultValue, _textLogFilesPath);
    _textLogFilesPath = MakePathAbsolute(rootPath, _textLogFilesPath);

    ReadJsonString(configurations, JsonPluginManifestsPath, PluginManifestPathDefaultValue, _pluginManifestsPath);
    _pluginManifestsPath = MakePathAbsolute(rootPath, _pluginManifestsPath);

    ReadJsonUInt(configurations, JsonSasExpiryInterval, SasExpiryIntervalDefaultValue, _sasExpiryInterval);
    ReadJsonUInt(configurations, JsonSasRenewalGracePeriod, SasRenewalGracePeriodDefaultValue, _sasRenewalGracePeriod);
    ReadJsonString(configurations, JsonDpsUri, DpsUriDefaultValue, _dpsUri);
    ReadDPSScopeId(configurations);
    ReadTPMSlotNumbers(configurations);
    ReadJsonString(configurations, JsonDebugConnectionString, DefaultDebugConnectionString, _debugConnectionString);
    ReadJsonString(configurations, JsonDmModuleId, DefaultModuleId, _dmModuleId);
    ReadModules(configurations, JsonOtherModuleIds, _otherModules);
    ReadMap(configurations, JsonHandlerConfigRoot, _handlerParametersMap);
}

string ServiceParameters::GetLogFilePath() const
{
    return _textLogFilesPath;
}

string ServiceParameters::GetPluginManifestPath() const
{
    return _pluginManifestsPath;
}

unsigned int ServiceParameters::GetSasTokenExpiryInterval() const
{
    return _sasExpiryInterval;
}

unsigned int ServiceParameters::GetSasRenewalGracePeriod() const
{
    return _sasRenewalGracePeriod;
}

string ServiceParameters::GetDpsUri() const
{
    return _dpsUri;
}

string ServiceParameters::GetDpsScopeId() const
{
    return _dpsScopeId;
}

unsigned int ServiceParameters::GetDmSlotNumber() const
{
    return _dmModuleSlotNumber;
}

unsigned int ServiceParameters::GetDeviceSlotNumber() const
{
    return _deviceSlotNumber;
}

string ServiceParameters::GetDebugConnectionString() const
{
    return _debugConnectionString;
}

const map<string, unsigned int>& ServiceParameters::GetOtherModules() const
{
    return _otherModules;
}

string ServiceParameters::GetDmModuleId() const
{
    return _dmModuleId;
}

bool ServiceParameters::GetHandlerParameters(const std::string& handlerId, Json::Value& handlerParameters) const
{
    std::map<std::string, Json::Value>::const_iterator it = _handlerParametersMap.find(handlerId);
    if (it != _handlerParametersMap.end())
    {
        handlerParameters = it->second;
        return true;
    }
    return false;
}

}}}}
