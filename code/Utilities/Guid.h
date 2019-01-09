// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <string>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {
    class Guid
    {
    public:
        static std::wstring NewGuid();
    };
        
}}}}
