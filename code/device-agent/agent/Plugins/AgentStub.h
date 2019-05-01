// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

#include "../../common/DMInterfaces.h"
#include "../../common/plugins/PluginInterfaces.h"
#include "../../common/plugins/CrossBinaryResponse.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    class AgentStub
    {
    public:

        static void SetMdmServer(std::shared_ptr<DMCommon::IMdmServer> iMdmServer);
        static void SetRawHandlerHost(std::shared_ptr<DMCommon::IRawHandlerHost> iPluginHost);

        //
        // ReverseInvoke()
        //
        // Inputs
        //   jsonInputString : json string of type CrossBinaryRequest.
        //
        // Returns:
        //   json string of type CrossBinaryResponse.
        //
        static int __stdcall ReverseInvoke(const char* jsonInputString, char** jsonOutputString);
        static int __stdcall ReverseDeleteBuffer(const char* buffer);

    private:

        //
        // ReverseInvoke()
        //  Serializes/Deserializes cross-binary objects and exceptions.
        //
        // Inputs
        //   jsonInputString : json string of type CrossBinaryRequest.
        //
        // Returns:
        //   json string of type CrossBinaryResponse.
        //
        static std::string ReverseInvoke(const std::string& jsonInputString) noexcept;

        //
        // ReverseInvoke()
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
        static Json::Value ReverseInvoke(
            const std::string& targetType,
            const std::string& targetId,
            const std::string& targetMethod,
            const Json::Value& targetParameters);

        // Mdm Server
        static Json::Value InvokeRunSyncML(const Json::Value& parameters);

        // Raw Handler Host
        static Json::Value InvokeReport(const Json::Value& parameters);
        static Json::Value InvokeSendEvent(const Json::Value& parameters);

        static std::shared_ptr<DMCommon::IMdmServer> _iMdmServer;
        static std::shared_ptr<DMCommon::IRawHandlerHost> _iPluginHost;
    };

}}}}
