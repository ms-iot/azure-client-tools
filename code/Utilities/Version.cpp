// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DMException.h"
#include "DMString.h"

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    vector<int> VersionFormatCheck(const string& version)
    {
        if (version.empty())
        {
            throw DMException(DMSubsystem::DeviceAgentPlugin, GetLastError(), "Version not found");
        }
        const int length = 3;
        vector<string> versionSplit;
        SplitString(version, '.', versionSplit);
        if (versionSplit.size() != length)
        {
            throw DMException(DMSubsystem::DeviceAgentPlugin, GetLastError(), "Version format is incorrect");
        }
        try
        {
            vector<int> a;
            for (string s : versionSplit)
            {
                a.push_back(stoi(s));
            }
            return a;
        }
        catch (const exception& e)
        {
            string msg;
            msg += "Version format is incorrect. Error: ";
            msg += e.what();
            throw DMException(DMSubsystem::DeviceAgentPlugin, GetLastError(), msg.c_str());
        }
    }

    int MajorVersionCompare(const string& v1, const string& v2)
    {
        vector<int> v1s = VersionFormatCheck(v1);
        vector<int> v2s = VersionFormatCheck(v2);

        if (v1s[0] > v2s[0]) {
            return 1;
        }
        if (v2s[0] > v1s[0]) {
            return -1;
        }
        return 0;
    }

}}}}