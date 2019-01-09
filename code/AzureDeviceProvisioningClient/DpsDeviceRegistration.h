// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

HRESULT DpsRegisterDeviceWithoutModules(
    UINT32 tpmDeviceSlotNumber,
    const std::wstring wdpsUri,
    const std::wstring wdpsScopeId
);

HRESULT DpsRegisterDeviceAndModules(
    UINT32 tpmDeviceSlotNumber,
    const std::string dpsUri,
    const std::string dpsScopeId,
    UINT32 tpmDmModuleSlotNumber,
    const std::string dmModuleId,
    const std::map<std::string, UINT32> otherModules
);
