// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "..\DMInterfaces.h"

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    class MdmProxy
    {
    public:
        MdmProxy();

        void SetMdmServer(std::shared_ptr<IMdmServer> iMdmServer);

        void RunSet(const std::string& cspPath, const std::string& value);

        void RunSet(const std::string& cspPath, int value);

        std::string RunGetString(const std::string& path);

        unsigned int RunGetUInt(const std::string& path);

        template<class T>
        bool TryGetNumber(const std::string& path, T& value)
        {
            bool success = true;
            try
            {
                value = static_cast<T>(RunGetUInt(path));
            }
            catch (typename DMUtils::DMException&)
            {
                // ToDo: should we log these in debug?
                success = false;
            }
            return success;
        }

        void RunExec(const std::string& path);

        void RunAddDataBase64(const std::string& path, const std::string& value);

        void RunAddData(const std::string& path, const std::string& value, const std::string& type);

        void RunDelete(const std::string& path);

    private:
        void RunSyncML(const std::string& inSyncML, std::string& outSyncML);

        std::shared_ptr<IMdmServer> _iMdmServer;
    };

}}}}
