// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// ToDo: use common paths
#include "..\..\AzureDeviceManagementCommon\DMInterfaces.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginInterfaces.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\CrossBinaryResponse.h"
#include "RawHandlerHostProxy.h"
#include "MdmServerProxy.h"
#include "RawHandlersStub.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Plugin { namespace Common {

    class PluginStub
    {
        typedef std::function<Json::Value(const Json::Value& parameters)> EntryPoint;
        typedef std::map<std::string, EntryPoint> FunctionMap;

    public:
        PluginStub();

        void Initialize();

        void RegisterRawHandler(
            const std::string& id,
            std::function<std::shared_ptr<DMCommon::IRawHandler>()> createFactory);

        //
        // Invoke()
        //  Serializes/Deserializes cross-binary objects and exceptions.
        //
        // Inputs
        //   jsonInputString : CrossBinaryRequest serialized into a json string.
        //
        // Returns:
        //   CrossBinaryResponse serialized into a json string.
        //
        std::string Invoke(const std::string& jsonInputString) noexcept;

        void SetReverseInvoke(DMCommon::PluginReverseInvokePtr ReverseInvoke_) noexcept;

        void SetReverseDeleteBuffer(DMCommon::PluginReverseDeleteBufferPtr ReverseDeleteBuffer_) noexcept;

    private:
        // Plugin Host Support
        void RegisterHostFunction(
            const std::string& targetMethod,
            const EntryPoint& entryPoint);

        Json::Value GetHandlersInfo() const;

        //
        // Invoke()
        //  Routes the data to its destination.
        //
        // Inputs
        //   targetType       : host, raw handler, etc.
        //   targetId         : raw handler id, etc.
        //   targetMethod     : target method name.
        //   targetParameters : parameters that can be unpacked by the target method.
        //
        // Returns:
        //   Json object returned by targetMethod.
        //
        Json::Value Invoke(
            const std::string& targetType,
            const std::string& targetId,
            const std::string& targetMethod,
            const Json::Value& targetParameters);

        void UnregisterRawHandler(
            const std::string& id);

        // Callback sites

        AgentBinaryProxy _agentBinaryProxy;
        std::shared_ptr<MdmServerProxy> _mdmServerProxy;
        std::shared_ptr<RawHandlerHostProxy> _rawHandlerHostProxy;

        RawHandlerStub _rawHandlerStub;

        // Data Members

        FunctionMap _hostFunctionMap;
    };

}}}}}
