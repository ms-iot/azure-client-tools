// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <functional>

typedef std::function<void(HANDLE, PTOKEN_USER)> TOKEN_HANDLER;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

class Shell
{
public:
    static HANDLE GetShellUserToken(unsigned int attemptCount = 1, unsigned int attemptDelay = 0);
    static std::wstring GetDmUserFolder();
};

}}}}