// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "..\DMConstants.h"
#include "SyncMLHelpers.h"

using namespace Microsoft::Azure::DeviceManagement::Utils;
using namespace Microsoft::WRL;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    void ReadXmlStructData(IStream* resultSyncML, ELEMENT_HANDLER handler)
    {
        wstring uriPath = L"SyncML\\SyncBody\\Results\\Item\\Source\\LocURI\\";
        wstring dataPath = L"SyncML\\SyncBody\\Results\\Item\\Data\\";
        wstring itemPath = L"SyncML\\SyncBody\\Results\\Item\\";

        wstring emptyString = L"";
        auto value = emptyString;
        auto uri = emptyString;

        ComPtr<IXmlReader> xmlReader;

        HRESULT hr = CreateXmlReader(__uuidof(IXmlReader), (void**)xmlReader.GetAddressOf(), NULL);
        if (FAILED(hr))
        {
            throw DMException(hr, "Error: Failed to create xml reader.");
        }

        hr = xmlReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);
        if (FAILED(hr))
        {
            throw DMException(hr, "Error: XmlReaderProperty_DtdProcessing() failed.");
        }

        hr = xmlReader->SetInput(resultSyncML);
        if (FAILED(hr))
        {
            throw DMException(hr, "Error: SetInput() failed.");
        }

        deque<wstring> pathStack;
        wstring currentPath;

        // Read until there are no more nodes
        XmlNodeType nodeType;
        while (S_OK == (hr = xmlReader->Read(&nodeType)))
        {
            switch (nodeType)
            {
            case XmlNodeType_Element:
            {
                const wchar_t* prefix = NULL;
                UINT prefixSize = 0;

                hr = xmlReader->GetPrefix(&prefix, &prefixSize);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetPrefix() failed.");
                }

                const wchar_t* localName;
                hr = xmlReader->GetLocalName(&localName, NULL);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetLocalName() failed.");
                }

                wstring elementName;
                if (prefixSize > 0)
                {
                    elementName = prefix;
                    elementName += L":";
                }
                elementName += localName;

                if (!xmlReader->IsEmptyElement())
                {
                    pathStack.push_back(elementName);

                    // rebuild the current path.
                    currentPath = L"";
                    for (auto& it : pathStack)
                    {
                        currentPath += it + L"\\";
                    }
                    if (itemPath == currentPath)
                    {
                        value = emptyString;
                        uri = emptyString;
                    }
                }
            }
            break;
            case XmlNodeType_EndElement:
            {
                const wchar_t* prefix = NULL;
                UINT prefixSize = 0;

                hr = xmlReader->GetPrefix(&prefix, &prefixSize);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetPrefix() failed.");
                }

                const wchar_t* localName = NULL;
                hr = xmlReader->GetLocalName(&localName, NULL);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetLocalName() failed.");
                }

                if (itemPath == currentPath)
                {
                    vector<wstring> uriTokens;
                    wstringstream ss(uri);
                    wstring s;

                    while (getline(ss, s, L'/'))
                    {
                        uriTokens.push_back(s);
                    }

                    handler(uriTokens, value);

                    value = emptyString;
                    uri = emptyString;
                }
                pathStack.pop_back();
                // rebuild the current path.
                currentPath = L"";
                for (auto& it : pathStack)
                {
                    currentPath += it + L"\\";
                }

            }
            break;
            case XmlNodeType_Text:
            case XmlNodeType_Whitespace:
            {
                const wchar_t* valueText = NULL;
                hr = xmlReader->GetValue(&valueText, NULL);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetValue() failed.");
                }

                if (uriPath == currentPath)
                {
                    uri = valueText;
                }
                else if (dataPath == currentPath)
                {
                    value = valueText;
                }
            }
            break;
            }
        }
    }

    void ReadXmlValue(IStream* resultSyncML, const wstring& targetXmlPath, wstring& value)
    {
        ComPtr<IXmlReader> xmlReader;

        HRESULT hr = CreateXmlReader(__uuidof(IXmlReader), (void**)xmlReader.GetAddressOf(), NULL);
        if (FAILED(hr))
        {
            throw DMException(hr, "Error: Failed to create xml reader.");
        }

        hr = xmlReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);
        if (FAILED(hr))
        {
            throw DMException(hr, "Error: XmlReaderProperty_DtdProcessing() failed.");
        }

        hr = xmlReader->SetInput(resultSyncML);
        if (FAILED(hr))
        {
            throw DMException(hr, "Error: SetInput() failed.");
        }

        deque<wstring> pathStack;
        wstring currentPath;

        // Read until there are no more nodes
        bool valueFound = false;
        bool pathFound = false;
        XmlNodeType nodeType;
        while (S_OK == (hr = xmlReader->Read(&nodeType)) && !valueFound)
        {
            switch (nodeType)
            {
            case XmlNodeType_Element:
            {
                const wchar_t* prefix = NULL;
                UINT prefixSize = 0;

                hr = xmlReader->GetPrefix(&prefix, &prefixSize);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetPrefix() failed.");
                }

                const wchar_t* localName;
                hr = xmlReader->GetLocalName(&localName, NULL);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetLocalName() failed.");
                }

                wstring elementName;
                if (prefixSize > 0)
                {
                    elementName = prefix;
                    elementName += L":";
                }
                elementName += localName;

                if (!xmlReader->IsEmptyElement())
                {
                    pathStack.push_back(elementName);

                    // rebuild the current path.
                    currentPath = L"";
                    for (auto& it : pathStack)
                    {
                        currentPath += it + L"\\";
                    }
                    if (targetXmlPath == currentPath)
                    {
                        pathFound = true;
                    }
                }
            }
            break;
            case XmlNodeType_EndElement:
            {
                const wchar_t* prefix = NULL;
                UINT prefixSize = 0;

                hr = xmlReader->GetPrefix(&prefix, &prefixSize);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetPrefix() failed.");
                }

                const wchar_t* localName = NULL;
                hr = xmlReader->GetLocalName(&localName, NULL);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetLocalName() failed.");
                }

                pathStack.pop_back();
            }
            break;
            case XmlNodeType_Text:
            case XmlNodeType_Whitespace:
            {
                const wchar_t* valueText = NULL;
                hr = xmlReader->GetValue(&valueText, NULL);
                if (FAILED(hr))
                {
                    throw DMException(hr, "Error: GetValue() failed.");
                }

                if (targetXmlPath == currentPath)
                {
                    value = valueText;
                    valueFound = true;
                }
            }
            break;
            }
        }

        if (!pathFound)
        {
            wstring message = targetXmlPath + L" not found.";
            throw DMException(ErrorMissingXmlPath, WideToMultibyte(message.c_str()));
        }
    }

    void ReadXmlStructData(const wstring& resultSyncML, ELEMENT_HANDLER handler)
    {
        DWORD bufferSize = static_cast<DWORD>(resultSyncML.size() * sizeof(resultSyncML[0]));
        char* buffer = (char*)GlobalAlloc(GMEM_FIXED, bufferSize);
        memcpy(buffer, resultSyncML.c_str(), bufferSize);

        ComPtr<IStream> dataStream;
        HRESULT hr = ::CreateStreamOnHGlobal(buffer, TRUE /*delete on release*/, dataStream.GetAddressOf());
        if (FAILED(hr))
        {
            GlobalFree(buffer);
            throw DMException(hr);
        }
        ReadXmlStructData(dataStream.Get(), handler);

        // GlobalFree() is not needed since 'delete on release' is enabled.
        // GlobalFree(buffer);
    }

    void ReadXmlValue(const wstring& resultSyncML, const wstring& targetXmlPath, wstring& value)
    {
        DWORD bufferSize = static_cast<DWORD>(resultSyncML.size() * sizeof(resultSyncML[0]));
        char* buffer = (char*)GlobalAlloc(GMEM_FIXED, bufferSize);
        memcpy(buffer, resultSyncML.c_str(), bufferSize);

        ComPtr<IStream> dataStream;
        HRESULT hr = ::CreateStreamOnHGlobal(buffer, TRUE /*delete on release*/, dataStream.GetAddressOf());
        if (FAILED(hr))
        {
            GlobalFree(buffer);
            throw DMException(hr);
        }
        ReadXmlValue(dataStream.Get(), targetXmlPath, value);

        // GlobalFree() is not needed since 'delete on release' is enabled.
        // GlobalFree(buffer);
    }

}}}}
