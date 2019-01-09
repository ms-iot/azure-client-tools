// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class PluginManifest
    {
    public:

        PluginManifest();

        void FromJsonFile(const std::string& manifestFileName);

        std::string GetCodeFileName() const;
        bool IsDirect() const;
        bool IsOutOfProc() const;
        long GetKeepAliveTime() const;

    private:

        std::string _codeFileName;
        bool _isDirect;
        bool _isOutOfProc;
        long _keepAliveTime;
    };

}}}}
