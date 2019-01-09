// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <stdio.h>
#include <string>
#include <vector>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {
    
    void LoadFile(const std::string& fileName, std::vector<char>& buffer);
    std::string FileToBase64(const std::string& fileName);

}}}}
