// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <Windows.h>
#include <sstream>
#include <ostream>
#include <iomanip>
#include "DateTime.h"
#include "DMException.h"

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

// ToDo: Provide Multi-byte versions.
wstring DateTime::GetCurrentDateTimeString()
{
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);

    return GetDateTimeString(systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
}

wstring DateTime::GetDateTimeString(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second)
{
    basic_ostringstream<wchar_t> formattedTime;
    formattedTime << setw(2) << setfill(L'0') << year
        << L'-' << setw(2) << setfill(L'0') << month
        << L'-' << setw(2) << setfill(L'0') << day
        << L'T' << setw(2) << setfill(L'0') << hour
        << L':' << setw(2) << setfill(L'0') << minute
        << L':' << setw(2) << setfill(L'0') << second;
    return formattedTime.str();
}

void DateTime::ISO8601DateTimeFromString(const wstring& dateTimeString, ISO8601DateTime& dateTime)
{
    // Iso 8601 partial spec
    // http://www.w3.org/TR/NOTE-datetime

    // ToDo: review and support more formats.
    // ToDo: support milliseconds.

    // YYYY-MM-DDTHH:MM:SS[Z]
    // YYYY-MM-DDTHH:MM:SS[(-|+)hh:mm]
    vector<wstring> tokens;
    Utils::SplitString(dateTimeString, L'T', tokens);
    if (tokens.size() != 2)
    {
        throw DMException(GetLastError(), "Error: invalid date/time format.");
    }

    vector<wstring> dateComponents;
    Utils::SplitString(tokens[0], L'-', dateComponents);
    if (dateComponents.size() != 3)
    {
        throw DMException(GetLastError(), "Error: invalid date/time format. Date must be in the form YYYY-MM-DD.");
    }
    dateTime.year = static_cast<WORD>(stoi(dateComponents[0]));
    dateTime.month = static_cast<WORD>(stoi(dateComponents[1]));
    dateTime.day = static_cast<WORD>(stoi(dateComponents[2]));

    wstring timeString = tokens[1];
    wstring zoneString;
    wchar_t zoneChar = '\0';
    vector<wstring> timeComponents;
    Utils::SplitString(timeString, L'-', timeComponents);
    if (timeComponents.size() == 2)
    {
        // 2016-10-10T09:00:01-008:00
        timeString = timeComponents[0];
        zoneChar = L'-';
        zoneString = timeComponents[1];
    }
    else
    {
        timeComponents.clear();
        Utils::SplitString(timeString, L'+', timeComponents);
        if (timeComponents.size() == 2)
        {
            // 2016-10-10T09:00:01+008:00
            timeString = timeComponents[0];
            zoneChar = L'+';
            zoneString = timeComponents[1];
        }
        else
        {
            if (timeString.size() > 0 &&
                (timeString[timeString.size() - 1] == L'Z' || timeString[timeString.size() - 1] == L'z'))
            {
                timeString = timeString.substr(0, timeString.size() - 1);
                zoneChar = L'Z';
            }
        }
        Utils::SplitString(tokens[1], L'-', timeComponents);
    }

    timeComponents.clear();
    Utils::SplitString(timeString, L':', timeComponents);
    if (timeComponents.size() != 3)
    {
        throw DMException(GetLastError(), "Error: invalid date/time format. Time must be in the form hh:mm:ss.");
    }
    dateTime.hour = static_cast<WORD>(stoi(timeComponents[0]));
    dateTime.minute = static_cast<WORD>(stoi(timeComponents[1]));
    dateTime.second = static_cast<WORD>(stoi(timeComponents[2]));
    dateTime.milliseconds = 0;

    timeComponents.clear();
    if (zoneChar == L'Z')
    {
        dateTime.zoneHour = 0;
        dateTime.zoneMinute = 0;
        dateTime.timeKind = TIME_KIND::UTC_TIME;
    }
    else if (zoneChar == '\0')
    {
        dateTime.zoneHour = 0;
        dateTime.zoneMinute = 0;
        dateTime.timeKind = TIME_KIND::LOCAL_TIME;
    }
    else
    {
        Utils::SplitString(zoneString, L':', timeComponents);
        if (timeComponents.size() != 2)
        {
            throw DMException(GetLastError(), "Error: invalid date/time format. Time offset information must be in the form hh:mm.");
        }
        dateTime.zoneHour = static_cast<short>(stoi(timeComponents[0])) * (zoneChar == L'-' ? -1 : 1);
        dateTime.zoneMinute = static_cast<WORD>(stoi(timeComponents[1]));
        dateTime.timeKind = TIME_KIND::UTC_TIME;
    }
}

void DateTime::SystemTimeFromISO8601(const wstring& dateTimeString, SYSTEMTIME& dateTime)
{
    ISO8601DateTime iso8601DateTime;
    ISO8601DateTimeFromString(dateTimeString, iso8601DateTime);

    if (iso8601DateTime.timeKind == TIME_KIND::UTC_TIME)
    {
        // We return false here because the time represented by SYSTEMTIME cannot capture
        // time zone information. It's always local time.
        throw DMException(GetLastError(), "Error: Only local time is allowed. Cannot specify time zone offset.");
    }

    dateTime.wYear = iso8601DateTime.year;
    dateTime.wMonth = iso8601DateTime.month;
    dateTime.wDay = iso8601DateTime.day;

    // ToDo: need to adjust the hour/minute to accommodate the time zone.
    dateTime.wHour = iso8601DateTime.hour;
    dateTime.wMinute = iso8601DateTime.minute;
    dateTime.wSecond = iso8601DateTime.second;
    dateTime.wMilliseconds = iso8601DateTime.milliseconds;
    dateTime.wDayOfWeek = 0;
}

wstring DateTime::ISO8601FromSystemTime(const SYSTEMTIME& dateTime)
{
    // Iso 8601 partial spec
    // http://www.w3.org/TR/NOTE-datetime

    // ToDo: review and support more formats.

    basic_ostringstream<wchar_t> formattedTime;
    formattedTime
        << setw(4) << setfill(L'0') << dateTime.wYear
        << L"-" << setw(2) << setfill(L'0') << dateTime.wMonth
        << L"-" << setw(2) << setfill(L'0') << dateTime.wDay
        << L"T"
        << setw(2) << setfill(L'0') << dateTime.wHour
        << L':' << setw(2) << setfill(L'0') << dateTime.wMinute
        << L':' << setw(2) << setfill(L'0') << dateTime.wSecond;

    return formattedTime.str();
}

}}}}