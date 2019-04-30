// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <string>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace CertificateManagementPlugin {

    class CertificateFile
    {
    public:
        CertificateFile(const std::string& certFileName);

        std::string GetFullFileName() const;
        std::string GetThumbPrint() const;

    private:
        std::string _fullFileName;
        std::string _thumbPrint;
    };

}}}}
