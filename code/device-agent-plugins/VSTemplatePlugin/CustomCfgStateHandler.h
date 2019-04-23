#pragma once

#include "device-agent\common\DMCommon.h"

namespace DMCommon = Microsoft::Azure::DeviceManagement::Common;

#define $safeprojectname$StateHandlerId "$safeprojectname$StateHandler"

class $safeprojectname$StateHandler : public DMCommon::HandlerBase
{
public:
    $safeprojectname$StateHandler();

    // IRawHandler
    void Start(
        const Json::Value& config,
        bool& active);

    void OnConnectionStatusChanged(
        DMCommon::ConnectionStatus status);

    DMCommon::InvokeResult Invoke(
        const Json::Value& groupDesiredConfigJson) noexcept;
};