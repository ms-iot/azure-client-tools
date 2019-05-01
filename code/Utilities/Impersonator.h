// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Windows.h>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class Impersonator
    {
    public:
        Impersonator();
        ~Impersonator();
        bool ImpersonateShellHost();
        HANDLE SIHostToken() { return _hToken; }

    private:
        void Close();
        HANDLE _hToken;
    };
}}}}