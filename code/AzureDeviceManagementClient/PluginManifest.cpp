// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PluginManifest.h"
#include "DMConstants.h"

using namespace std;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    PluginManifest::PluginManifest()
    {
    }

    void PluginManifest::FromJsonFile(const std::string& manifestFileName)
    {
        Json::Value value = JsonUtils::ParseJSONFile(manifestFileName);

        if (value.isNull() || !value.isObject())
        {
            throw DMException(AGENT_ERR_PLUGIN_MANIFEST_INVALID_SCHEMA, "Invalid json schema - plug-in manifest file.");
        }

        Json::Value codeFileName = value[JsonPluginCodeFileName];
        if (codeFileName.isNull() || !codeFileName.isString())
        {
            throw DMException(AGENT_ERR_PLUGIN_MANIFEST_MISSING_CODE_FILENAME, "Invalid json schema - plug-in manifest file missing binary `file name`.");
        }

        _codeFileName = codeFileName.asString();

        Json::Value isDirect = value[JsonPluginDirect];
        if (isDirect.isNull() || !isDirect.isBool())
        {
            throw DMException(AGENT_ERR_PLUGIN_MANIFEST_MISSING_OR_INVALID_DIRECT_ATTRIBUTE, "Invalid json schema - plug-in manifest file missing attribute `direct` or has an invalid type.");
        }

        _isDirect = isDirect.asBool();

        if (!_isDirect)
        {
            Json::Value isOutOfProc = value[JsonPluginOutOfProc];
            if (isOutOfProc.isNull() || !isOutOfProc.isBool())
            {
                throw DMException(AGENT_ERR_PLUGIN_MANIFEST_MISSING_OR_INVALID_OUTOFPROC_ATTRIBUTE, "Invalid json schema - plug-in manifest file missing attribute `outOfProc` or has an invalid type.");
            }

            _isOutOfProc = isOutOfProc.asBool();

            if (_isOutOfProc)
            {
                Json::Value keepAliveTime = value[JsonPluginKeepAliveTime];
                if (keepAliveTime.isNull() || !keepAliveTime.isUInt())
                {
                    throw DMException(AGENT_ERR_PLUGIN_MANIFEST_MISSING_OR_INVALID_KEEPALIVE_ATTRIBUTE, "Invalid json schema - plug-in manifest file missing attribute `keepAliveTime` or has an invalid type.");
                }

                _keepAliveTime = keepAliveTime.asUInt();
            }
        }
    }

    std::string PluginManifest::GetCodeFileName() const
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

}}}}
