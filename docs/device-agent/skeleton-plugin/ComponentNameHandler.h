#pragma once

namespace Microsoft { namespace Windows { namespace Azure { namespace DeviceManagement { namespace ComponentNamePlugin {

    class ComponentNameHandler : public DMCommon::BaseHandler
    {
    public:
        ComponentNameHandler();

        int Start(
            const Json::Value& handlerConfig);

        DMCommon::InvokeResult Invoke(
            const Json::Value& desiredDeltaConfig);
    };

}}}}}