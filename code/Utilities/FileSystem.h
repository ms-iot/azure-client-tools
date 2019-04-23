// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <vector>
#include <set>
#include <filesystem>

using namespace std::experimental::filesystem;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    typedef std::function<void(HANDLE, PTOKEN_USER)> TOKEN_HANDLER;
    std::wstring GetTempFolder();
    std::wstring GetTempFileW(const std::wstring& prefix);
    std::wstring GetSystemRootFolderW();
    std::string GetProcessPath();
    std::string MakePathAbsolute(const std::string& root, const std::string& fileName);
    void EnsureFolderExists(const std::string& folder);
    std::vector<std::string> GetFileSystemObjectNames(const std::wstring& path, file_type type);

}}}}