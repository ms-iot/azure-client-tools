// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include <string>

struct ApplicationInfo
{
    std::string packageFamilyName;
    std::string name;
    std::string version;
    std::string installDate;
    std::string appSource;
    bool startup;

    ApplicationInfo(const std::string& pkgFamilyName, const std::string& appSource) :
        packageFamilyName(pkgFamilyName),
        appSource(appSource)
    {}

    ApplicationInfo(const std::string& pkgFamilyName, const std::string& appSource, const std::string& appName, const std::string& appVersion, const std::string& appInstallDate) :
        packageFamilyName(pkgFamilyName),
        appSource(appSource),
        name(appName),
        version(appVersion),
        installDate(appInstallDate)
    {}

};