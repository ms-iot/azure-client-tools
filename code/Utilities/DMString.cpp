// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <windows.h>
#include <wincrypt.h>
#include "DMString.h"
#include "DMException.h"

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    string WideToMultibyte(const wchar_t* s)
    {
        size_t length = s ? wcslen(s) : 0;
        size_t requiredCharCount = WideCharToMultiByte(CP_UTF8, 0, s, static_cast<int>(length), nullptr, 0, nullptr, nullptr);

        // add room for \0
        ++requiredCharCount;

        vector<char> multibyteString(requiredCharCount);
        WideCharToMultiByte(CP_UTF8, 0, s, static_cast<int>(length), multibyteString.data(), static_cast<int>(multibyteString.size()), nullptr, nullptr);

        return string(multibyteString.data());
    }

    wstring MultibyteToWide(const char* s)
    {
        size_t length = s ? strlen(s) : 0;
        size_t requiredCharCount = MultiByteToWideChar(CP_UTF8, 0, s, static_cast<int>(length), nullptr, 0);

        // add room for \0
        ++requiredCharCount;

        vector<wchar_t> wideString(requiredCharCount);
        MultiByteToWideChar(CP_UTF8, 0, s, static_cast<int>(length), wideString.data(), static_cast<int>(wideString.size()));

        return wstring(wideString.data());
    }

    wstring TrimString(const std::wstring& s, const std::wstring& suffix)
    {
        wstring trimmed = s;

        size_t pos = s.find(suffix);
        if (wstring::npos != pos && pos == s.length() - suffix.length())
        {
            trimmed = s.substr(0, s.length() - suffix.length());
        }
        return trimmed;
    }

    bool Contains(const wstring& container, const wstring& contained)
    {
        if (container.size() < contained.size())
        {
            return false;
        }

        bool match = false;
        for (size_t i = 0; (i < container.size() - contained.size() + 1) && !match; ++i)
        {
            match = true;
            for (size_t j = 0; j < contained.size(); ++j)
            {
                if (towlower(container[i + j]) != towlower(contained[j]))
                {
                    match = false;
                    break;
                }
            }
        }
        return match;
    }

    void Base64ToBinary(const string& base64string, vector<unsigned char>& buffer)
    {
        DWORD destinationSize = 0;
        if (!CryptStringToBinaryA(base64string.c_str(), static_cast<unsigned int>(base64string.size()), CRYPT_STRING_BASE64, nullptr, &destinationSize, nullptr, nullptr))
        {
            throw DMException(DMSubsystem::Windows, GetLastError(),  "Error: cannot obtain the required size to decode buffer from base64.");
        }

        buffer.resize(destinationSize);
        if (!CryptStringToBinaryA(base64string.c_str(), static_cast<unsigned int>(base64string.size()), CRYPT_STRING_BASE64, reinterpret_cast<unsigned char*>(buffer.data()), &destinationSize, nullptr, nullptr))
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: cannot obtain the required size to decode buffer from base64.");
        }
    }

    std::string Base64ToString(const std::string& base64string)
    {
        vector<unsigned char> buffer;
        Base64ToBinary(base64string, buffer);

        string base64String = reinterpret_cast<char*>(buffer.data());

        return base64String;
    }

    string BinaryToBase64(const vector<unsigned char>& buffer)
    {
        unsigned char* nonConstBuffer = const_cast<unsigned char*>(buffer.data());

        DWORD destinationSize = 0;
        if (!CryptBinaryToStringA(nonConstBuffer, static_cast<unsigned int>(buffer.size()), CRYPT_STRING_BASE64, nullptr, &destinationSize))
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: cannot obtain the required size to encode buffer into base64.");
        }

        vector<char> destinationBuffer(destinationSize);
        if (!CryptBinaryToStringA(nonConstBuffer, static_cast<unsigned int>(buffer.size()), CRYPT_STRING_BASE64, destinationBuffer.data(), &destinationSize))
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: cannot convert binary stream to base64.");
        }

        // Note that the size returned includes the null terminating character.
        string base64string(destinationBuffer.data(), destinationBuffer.size() - 1);

        return base64string;
    }

    std::string StringToBase64(const std::string& bufferString)
    {
        size_t bufferStringSize = bufferString.size() + 1;

        vector<unsigned char> buffer(bufferStringSize);
        memcpy(buffer.data(), bufferString.c_str(), bufferStringSize);

        return BinaryToBase64(buffer);
    }

}}}}