// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Impersonator.h"
#include "Logger.h"
#include "Shell.h"

#include <tlhelp32.h>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    Impersonator::Impersonator() :
        _hToken(nullptr)
    {
    }

    Impersonator::~Impersonator()
    {
        Close();
    }

    bool Impersonator::ImpersonateShellHost()
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        Close();

        _hToken = Shell::GetShellUserToken();

        if (!ImpersonateLoggedOnUser(_hToken))
        {
           TRACELINE(LoggingLevel::Verbose, "Error: Failed to impersonate user...");
           Close();
           return false;
        }

        TRACELINE(LoggingLevel::Verbose, "Impersonating succeeded!");
        return true;
    }

    void Impersonator::Close()
    {
        RevertToSelf();

        if (_hToken)
        {
            CloseHandle(_hToken);
            _hToken = nullptr;
        }
    }
}}}}