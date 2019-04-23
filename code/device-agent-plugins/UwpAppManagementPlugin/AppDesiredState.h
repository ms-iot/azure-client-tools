// Copyright (c) Microsft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "stdafx.h"

enum StartupType
{
    eUndefined = 0,
    eNone,
    eForeground,
    eBackground
};

enum AppDesiredAction
{
    eQuery = 0,
    eInstall,
    eUninstall,
    eUpgrade,
    eDowngrade
};

struct AppDesiredState
{
    std::string packageFamilyId;
    std::string packageFamilyName;
    Version version;
    AppDesiredAction action;
    StartupType startupType;
    std::string appxSource;
    std::string depsSources;
    bool launchAfterInstall;
    Windows::ApplicationModel::Package^ package;

    AppDesiredState(const std::string pkgFamilyId) : 
        packageFamilyId(pkgFamilyId),
        startupType(StartupType::eUndefined),
        action(AppDesiredAction::eQuery),
        version("0.0.0.0"),
        package(nullptr),
        launchAfterInstall(false)
    {}
};

typedef std::vector<AppDesiredState> AppDesiredStateList;