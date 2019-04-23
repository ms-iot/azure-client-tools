// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>
#include "../common/DMInterfaces.h"
#include "../common/plugins/PluginInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class PluginHostStub
    {
    public:
        static void SetPluginTransport(std::shared_ptr<DMCommon::IPluginTransport> transport);
        static int __stdcall ReverseInvoke(const char* jsonInputString, char** jsonOutputString);
        static int __stdcall ReverseDeleteBuffer(const char* buffer);
    private:
        static std::shared_ptr<DMCommon::IPluginTransport> _transport;
    };
}}}}
