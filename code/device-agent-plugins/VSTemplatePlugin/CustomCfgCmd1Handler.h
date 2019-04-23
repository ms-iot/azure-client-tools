#pragma once

#include "device-agent\common\DMCommon.h"

namespace DMCommon = Microsoft::Azure::DeviceManagement::Common;

#define $safeprojectname$Cmd1HandlerId "$safeprojectname$Cmd1Handler"

class $safeprojectname$Cmd1Handler : public DMCommon::HandlerBase
{
public:
    $safeprojectname$Cmd1Handler();

    // IRawHandler
    void Start(
        const Json::Value& config,
        bool& active);

    void OnConnectionStatusChanged(
        DMCommon::ConnectionStatus status);

    DMCommon::InvokeResult Invoke(
        const Json::Value& groupDesiredConfigJson) noexcept;
};