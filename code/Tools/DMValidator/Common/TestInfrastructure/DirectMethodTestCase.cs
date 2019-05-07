// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace DMValidator
{
    class DirectMethodTestCase : TestCase
    {
        public static DirectMethodTestCase FromJson(ILogger logger, JObject testCaseJson)
        {
            DirectMethodTestCase testCase = new DirectMethodTestCase();

            TestCase.FromJson(logger, testCaseJson, testCase);

            testCase._methodName = JsonHelpers.GetString(testCaseJson, Constants.JsonDirectMethodName);

            // Input
            testCase._parameters = JsonHelpers.GetObject(testCaseJson, Constants.TCJsonInput);

            // Output
            JObject output = JsonHelpers.GetObject(testCaseJson, Constants.TCJsonOutput);

            // Output - DeviceTwin
            JObject deviceTwin = null;
            if (JsonHelpers.TryGetObject(output, Constants.TCJsonMethodDeviceTwin, out deviceTwin))
            {
                if (JsonHelpers.TryGetObject(output, Constants.TCJsonOutputPresent, out testCase._expectedPresentReportedState))
                {
                    testCase._expectedPresentReportedState = (JObject)testCase._expectedPresentReportedState.DeepClone();
                }

                if (JsonHelpers.TryGetObject(output, Constants.TCJsonOutputAbsent, out testCase._expectedAbsentReportedState))
                {
                    testCase._expectedAbsentReportedState = (JObject)testCase._expectedAbsentReportedState.DeepClone();
                }
            }

            // Output - Direct Method Return
            JsonHelpers.TryGetToken(output, Constants.TCJsonMethodReturnJson, out testCase._expectedReturnJson);
            JsonHelpers.TryGetLong(output, Constants.TCJsonMethodReturnCode, out testCase._expectedReturnCode);

            return testCase;
        }

        public override async Task Execute(ILogger logger, TestParameters testParameters)
        {
            JObject resolvedParameters = (JObject)testParameters.ResolveParameters(_parameters);

            DeviceMethodReturnValue returnValue = await testParameters.IoTCloudServices.IoTHubManager.InvokeDirectMethod(
                testParameters.IoTHubDeviceId,
                _methodName,
                resolvedParameters.ToString());

            if (returnValue.Status != _expectedReturnCode)
            {
                LogMismatch(logger, _expectedReturnCode.ToString(), returnValue.Status.ToString());
                ReportResult(logger, false, "Unexpected direct method return value.");
            }

            JToken actualReturnJson = JValue.Parse(returnValue.Payload);

            List<string> errorList = new List<string>();
            if (!TestCaseHelpers.VerifyPropertiesPresent("returnPayload", _expectedReturnJson, actualReturnJson, errorList))
            {
                logger.Log(LogLevel.Verbose, "---- Final Result:");
                logger.Log(LogLevel.Verbose, returnValue.Payload.ToString());
                logger.Log(LogLevel.Verbose, "---- Expected Present Result:");
                logger.Log(LogLevel.Verbose, _expectedReturnJson.ToString());
                ReportResult(logger, false, "Unexpected direct method return payload.");
            }
        }

        private string _methodName;
        private JObject _parameters;
        private JObject _expectedPresentReportedState;
        private JObject _expectedAbsentReportedState;
        private JToken _expectedReturnJson;
        private long _expectedReturnCode;
    }
}
