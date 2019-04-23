// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

class Dps
{
public:
    static HRESULT SetScopeId(_In_ const std::wstring& scopeId);

private:
    static std::mutex _setScopeIdMutex;
};