// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "PluginNamedPipeBaseTransport.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class PluginNamedPipePluginTransport : public PluginNamedPipeBaseTransport
    {
    public:
        PluginNamedPipePluginTransport(
            const std::wstring& pluginPath,
            const std::wstring& clientPipeName,
            const std::wstring& pluginPipeName);

        ~PluginNamedPipePluginTransport();

    private:

        // IPluginTransport

        void Initialize();

        void SetClientInterface(
            PluginReverseInvokePtr reverseInvokePtr,
            PluginReverseDeleteBufferPtr reverseDeletePtr);

        void SetPluginInterface(
            PluginCreatePtr createPtr,
            PluginInvokePtr invokePtr,
            PluginDeleteBufferPtr deleteBufferPtr);

        // PluginNamedPipeBaseTransport virtuals

        void ProcessRequestMessage(
            std::shared_ptr<Message> requestMessage);

        // Data members

        PluginCreatePtr _pluginCreate;
        PluginInvokePtr _pluginInvoke;
        PluginDeleteBufferPtr _pluginDeleteBuffer;
    };

}}}}
