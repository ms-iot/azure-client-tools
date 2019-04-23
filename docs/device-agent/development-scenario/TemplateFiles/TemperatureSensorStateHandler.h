#pragma once

#include "AzureDeviceManagementCommon\DMCommon.h"
namespace DMCommon = Microsoft::Azure::DeviceManagement::Common;

#define TemperatureSensorStateHandlerId "TemperatureSensorStateHandler"

class TemperatureSensorStateHandler : public DMCommon::BaseHandler
{
public:
    TemperatureSensorStateHandler();

    // IRawHandler
    void Start(
        const Json::Value& config,
        bool& active);

    void OnConnectionStatusChanged(
        DMCommon::ConnectionStatus status);

    DMCommon::InvokeResult Invoke(
        const Json::Value& groupDesiredConfigJson) noexcept;
};
