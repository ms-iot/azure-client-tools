// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <vector>
#include <set>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    std::wstring GetTempFolder();
    std::wstring GetSystemRootFolderW();
    std::string GetProcessPath();
    std::string MakePathAbsolute(const std::string& root, const std::string& fileName);

}}}}

