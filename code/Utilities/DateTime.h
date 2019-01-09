// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <Windows.h>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    enum TIME_KIND { LOCAL_TIME, UTC_TIME };

    struct ISO8601DateTime
    {
        unsigned short year;
        unsigned short month;
        unsigned short day;
        unsigned short hour;
        unsigned short minute;
        unsigned short second;
        unsigned short milliseconds;
        short zoneHour;
        unsigned short zoneMinute;

        // timeKind indicates whether this structure holds the necessary information to construct a UTC time
        // or just a local time.
        // Note that to construct the correct UTC time, you need to take into account the zoneHour and zoneMinute.
        // Note that if timeKind is LOCAL_TIME, zoneHour and zoneMinute will be 0s.
        TIME_KIND timeKind;

        ISO8601DateTime() :
            year(0),
            month(0),
            day(0),
            hour(0),
            minute(0),
            second(0),
            milliseconds(0),
            zoneHour(0),
            zoneMinute(0),
            timeKind(LOCAL_TIME)
        {}
    };

    class DateTime
    {
    public:

        static void SystemTimeFromISO8601(const std::wstring& dateTimeString, SYSTEMTIME& dateTime);
        static std::wstring ISO8601FromSystemTime(const SYSTEMTIME& dateTime);

        static void ISO8601DateTimeFromString(const std::wstring& dateTimeString, ISO8601DateTime& dateTime);

        static std::wstring GetCurrentDateTimeString();
        static std::wstring GetDateTimeString(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);
    };

}}}}
