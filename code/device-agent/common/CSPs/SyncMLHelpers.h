// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    typedef std::function<void(std::vector<std::wstring>&, std::wstring&)>& ELEMENT_HANDLER;

    void ReadXmlValue(const std::wstring& resultSyncML, const std::wstring& targetXmlPath, std::wstring& value);
    void ReadXmlStructData(const std::wstring& resultSyncML, ELEMENT_HANDLER handler);
    void ReadXmlStatus(const std::wstring& resultSyncML, const std::wstring& targetXmlPath, std::vector<unsigned int>& returnCodes);

}}}}
