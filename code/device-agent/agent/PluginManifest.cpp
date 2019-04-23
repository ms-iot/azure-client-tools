// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginManifest.h"
#include "DMConstants.h"

using namespace std;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    HandlerConfiguration::HandlerConfiguration(const Json::Value& value)
    {
        Json::Value handlerIdSetting = value[JsonPluginHandlerId];
        if (handlerIdSetting.isNull() || !handlerIdSetting.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_PLUGIN_MANIFEST_MISSING_CODE_PUBLISH, "Invalid json schema - plug-in manifest file missing attribute `id`.");
        }
        _id = handlerIdSetting.asString();

        Json::Value dependencyList = value[JsonPluginHandlerDependencyList];
        if (!dependencyList.isNull())
        {
            if (!dependencyList.isArray())
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_PLUGIN_MANIFEST_MISSING_CODE_DEVICES, "Invalid json schema - plug-in manifest contains 'dependencyList', but is not an Array.");
            }

            for (auto it = dependencyList.begin(); it != dependencyList.end(); ++it)
            {
                _dependencies.push_back((*it).asString());
            }
        }
    }

    string HandlerConfiguration::GetId() const
    {
        return _id;
    }
    vector<string> HandlerConfiguration::GetDependencies() const
    {
        return _dependencies;
    }

    PluginManifest::PluginManifest()
    {
    }

    void PluginManifest::FromJsonFile(const string& manifestFileName)
    {
        Json::Value value = JsonUtils::ParseJSONFile(manifestFileName);

        if (value.isNull() || !value.isObject())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_INVALID_MANIFEST_SCHEMA, "Invalid json schema - plug-in manifest file.");
        }

        Json::Value codeFileName = value[JsonPluginCodeFileName];
        if (codeFileName.isNull() || !codeFileName.isString())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MANIFEST_MISSING_CODE_FILENAME, "Invalid json schema - plug-in manifest file missing binary `file name`.");
        }

        _codeFileName = codeFileName.asString();

        Json::Value isDirect = value[JsonPluginDirect];
        if (isDirect.isNull() || !isDirect.isBool())
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MANIFEST_MISSING_DIRECT_ATTRIBUTE, "Invalid json schema - plug-in manifest file missing attribute `direct` or has an invalid type.");
        }

        _isDirect = isDirect.asBool();

        if (!_isDirect)
        {
            Json::Value isOutOfProc = value[JsonPluginOutOfProc];
            if (isOutOfProc.isNull() || !isOutOfProc.isBool())
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MANIFEST_MISSING_OUTOFPROC_ATTRIBUTE, "Invalid json schema - plug-in manifest file missing attribute `outOfProc` or has an invalid type.");
            }

            _isOutOfProc = isOutOfProc.asBool();

            if (_isOutOfProc)
            {
                Json::Value keepAliveTime = value[JsonPluginKeepAliveTime];
                if (keepAliveTime.isNull() || !keepAliveTime.isInt())
                {
                    throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MANIFEST_MISSING_KEEPALIVE_ATTRIBUTE, "Invalid json schema - plug-in manifest file missing attribute `keepAliveTime` or has an invalid type.");
                }

                _keepAliveTime = keepAliveTime.asInt();
                if (_keepAliveTime < -1)
                {
                    throw DMException(DMSubsystem::DeviceAgent, DM_PLUGIN_ERROR_MANIFEST_INVALID_KEEPALIVE_ATTRIBUTE, "Invalid json schema - plug-in manifest file invalid attribute `keepAliveTime` value.");
                }
            }
        }

        Json::Value handlersSetting = value[JsonPluginHandlers];
        if (!handlersSetting.isNull())
        {
            if (!handlersSetting.isArray())
            {
                throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_PLUGIN_MANIFEST_MISSING_CODE_DEVICES, "Invalid json schema - plug-in manifest contains 'handlers', but is not an Array.");
            }

            for (auto it = handlersSetting.begin(); it != handlersSetting.end(); ++it)
            {
                auto handlerInfo = make_shared<HandlerConfiguration>(*it);
                auto handlerId = handlerInfo->GetId();
                _handlerInfoList[handlerId] = handlerInfo;
            }

        }
    }

    string PluginManifest::GetCodeFileName() const
    {
        return _codeFileName;
    }

    bool PluginManifest::IsDirect() const
    {
        return _isDirect;
    }

    bool PluginManifest::IsOutOfProc() const
    {
        return _isOutOfProc;
    }

    long PluginManifest::GetKeepAliveTime() const
    {
        return _keepAliveTime;
    }

    const map<string, shared_ptr<HandlerConfiguration>>& PluginManifest::GetHandlers() const
    {
        return _handlerInfoList;
    }

}}}}