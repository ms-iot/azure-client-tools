// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class Version
    {
    private:
        int major = 0, minor = 0, revision = 0, build = 0;

    public:
        Version(const std::string& version)
        {
            sscanf_s(version.c_str(), "%d.%d.%d.%d", &major, &minor, &revision, &build);
        }

        bool operator < (const Version& version)
        {
            if (major < version.major)
                return true;
            if (minor < version.minor)
                return true;
            if (revision < version.revision)
                return true;
            if (build < version.build)
                return true;
            return false;
        }

        bool operator == (const Version& version)
        {
            return major == version.major
                && minor == version.minor
                && revision == version.revision
                && build == version.build;
        }
    };

    std::vector<int> VersionFormatCheck(const std::string& version);

    int MajorVersionCompare( const std::string& v1, const std::string& v2);

}}}}