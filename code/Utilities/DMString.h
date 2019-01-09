// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <vector>
#include <set>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    std::string WideToMultibyte(const wchar_t* s);
    std::wstring MultibyteToWide(const char* s);

    bool Contains(const std::wstring& container, const std::wstring& contained);

    template<class T>
    void SplitString(const std::basic_string<T> &s, T delim, std::vector<std::basic_string<T>>& tokens)
    {
        std::basic_stringstream<T> ss;
        ss.str(s);
        std::basic_string<T> item;
        while (std::getline<T>(ss, item, delim))
        {
            tokens.push_back(item);
        }
    }

    template<class T>
    void SplitString(const std::basic_string<T> &s, T delim, std::set<std::basic_string<T>>& tokens)
    {
        std::basic_stringstream<T> ss;
        ss.str(s);
        std::basic_string<T> item;
        while (std::getline<T>(ss, item, delim))
        {
            tokens.emplace(item);
        }
    }

    template<class T>
    T TrimString(const T& s, const T& chars)
    {
        T trimmedString;

        // trim leading characters
        size_t startpos = s.find_first_not_of(chars);
        if (T::npos != startpos)
        {
            trimmedString = s.substr(startpos);
        }

        // trim trailing characters
        size_t endpos = trimmedString.find_last_not_of(chars);
        if (T::npos != endpos)
        {
            trimmedString = trimmedString.substr(0, endpos + 1);
        }
        return trimmedString;
    }

    std::wstring TrimString(const std::wstring& s, const std::wstring& suffix);

    template<class CharType, class ParamType>
    std::basic_string<CharType> ConcatString(const CharType* s, ParamType param)
    {
        std::basic_ostringstream<CharType> messageStream;
        messageStream << s << param;
        return messageStream.str();
    }

    void Base64ToBinary(const std::string& base64string, std::vector<unsigned char>& buffer);
    std::string Base64ToString(const std::string& base64string);
    std::string BinaryToBase64(const std::vector<unsigned char>& buffer);
    std::string StringToBase64(const std::string& bufferString);

}}}}
