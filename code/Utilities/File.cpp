// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <windows.h>
#include "DMException.h"
#include <WinCrypt.h>

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    void LoadFile(const string& fileName, vector<char>& buffer)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINEP(LoggingLevel::Verbose, "fileName = ", fileName.c_str());

        ifstream file(fileName, ios::in | ios::binary | ios::ate);

        string line;
        if (!file.is_open())
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: failed to open binary file!");
        }

        buffer.resize(static_cast<unsigned int>(file.tellg()));
        file.seekg(0, ios::beg);
        if (!file.read(buffer.data(), buffer.size()))
        {
            throw DMException(DMSubsystem::Windows, GetLastError(), "Error: failed to read file!");
        }
        file.close();
    }

    string FileToBase64(const string& fileName)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        TRACELINEP(LoggingLevel::Verbose, "fileName = ", fileName.c_str());

        vector<char> buffer;
        LoadFile(fileName, buffer);

        vector<unsigned char> buff(buffer.size());
        memcpy(buff.data(), buffer.data(), buffer.size());

        return BinaryToBase64(buff);
    }

 }}}}
