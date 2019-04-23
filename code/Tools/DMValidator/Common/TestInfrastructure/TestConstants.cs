// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace DMValidator
{
    static class Constants
    {
        public const string JsonPropertiesRoot = "properties";
        public const string JsonDesiredRoot = "desired";
        public const string JsonReportedRoot = "reported";
        public const string JsonDeviceTwin = "deviceTwin";
        public const string JsonDirectMethodEmptyParams = "{}";
        public const string JsonClearReportedCmd = "clearReportedCmd";

        public const string JsonScenario = "scenario";

        public const string JsonDirectMethodName = "directMethodName";

        public const string TCJsonName = "name";
        public const string TCJsonDescription = "description";
        public const string TCJsonReadBackPause = "readBackPause";
        public const int    TCJsonReadBackPauseDefault = 10;      // In seconds
        public const string TCJsonInput = "input";
        public const string TCJsonOutput = "output";
        public const string TCJsonOutputPresent = "present";
        public const string TCJsonOutputAbsent = "absent";

        public const string TCJsonType = "type";
        public const string TCJsonTypeInteractionDeviceTwin = "deviceTwin";
        public const string TCJsonTypeInteractionDirectMethod = "directMethod";
        public const string TCJsonTypeInteractionDotNetApi = "dotNetApi";

        public const string TCJsonMethodReturnJson = "returnJson";
        public const string TCJsonMethodReturnCode = "returnCode";
        public const string TCJsonMethodDeviceTwin = "deviceTwin";

        public const string TCJsonDotNetApiName = "apiName";

        public const string TCJsonSetWindowsUpdateRingAsync = "SetWindowsUpdateRingAsync";

        public const string JsonIgnore = "<ignore>";

        public const string JsonDeviceInfo = "deviceInfo";
        public const string JsonRebootInfo = "rebootInfo";
        public const string JsonTimeInfo = "timeInfo";
        public const string JsonWindowsUpdate = "windowsUpdate";
        public const string JsonWindowsTelemetry = "windowsTelemetry";

    }
}
