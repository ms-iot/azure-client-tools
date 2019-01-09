// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <utility>
#include "AutoCloseBase.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class AutoCloseHandle : public AutoCloseBase<HANDLE>
    {
    public:
        AutoCloseHandle() :
            AutoCloseBase(NULL, [](HANDLE h) { CloseHandle(h); return TRUE; })
        {}

        AutoCloseHandle(HANDLE&& handle) :
            AutoCloseBase(std::move(handle), [](HANDLE h) { CloseHandle(h); return TRUE; })
        {}

    private:
        AutoCloseHandle(const AutoCloseHandle &);            // prevent copy
        AutoCloseHandle& operator=(const AutoCloseHandle&);  // prevent assignment
    };

}}}}
