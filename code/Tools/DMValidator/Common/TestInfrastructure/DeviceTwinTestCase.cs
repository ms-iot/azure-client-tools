// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace DMValidator
{
    class DeviceTwinTestCase : TestCase
    {
        public static DeviceTwinTestCase FromJson(ILogger logger, JObject testCaseJson)
        {
            DeviceTwinTestCase testCase = new DeviceTwinTestCase();

            TestCase.FromJson(logger, testCaseJson, testCase);

            // Input
            testCase._desiredState = JsonHelpers.GetObject(testCaseJson, Constants.TCJsonInput);

            // Output
            JObject output = JsonHelpers.GetObject(testCaseJson, Constants.TCJsonOutput);

            JObject expectedPresentReportedState = JsonHelpers.GetObject(output, Constants.TCJsonOutputPresent);
            testCase._expectedPresentReportedState = (JObject)expectedPresentReportedState.DeepClone();

            JObject expectedAbsentReportedState;
            if (JsonHelpers.TryGetObject(output, Constants.TCJsonOutputAbsent, out expectedAbsentReportedState))
            {
                testCase._expectedAbsentReportedState = (JObject)expectedAbsentReportedState.DeepClone();
            }

            return testCase;
        }

        public override async Task Execute(ILogger logger, TestParameters testParameters)
        {
            JObject resolvedDesiredState = (JObject)testParameters.ResolveParameters(_desiredState);
            JToken desiredNode = resolvedDesiredState[Constants.JsonPropertiesRoot][Constants.JsonDesiredRoot];
            if (desiredNode is JObject)
            {
                JObject desiredJObject = (JObject)desiredNode;
                await testParameters.IoTCloudServices.IoTHubManager.UpdateDesiredObject(testParameters.IoTHubDeviceId, desiredJObject);
            }
            else
            {
                throw new Exception("DeviceTwin Test Case: Unexpected format!");
            }

            logger.Log(LogLevel.Information, IndentInner + "Waiting " + _readBackPause + " seconds for the device twin to be updated...");
            await Task.Delay(_readBackPause * 1000);

            DeviceData deviceData = await testParameters.IoTCloudServices.IoTHubManager.GetDeviceData(testParameters.IoTHubDeviceId);

            JObject desiredProperties = (JObject)JsonConvert.DeserializeObject(deviceData.desiredPropertiesJson);
            JObject reportedProperties = (JObject)JsonConvert.DeserializeObject(deviceData.reportedPropertiesJson);

            JObject expectedWindowsReported = (JObject)_expectedPresentReportedState[Constants.JsonPropertiesRoot][Constants.JsonReportedRoot];

            List<string> errorList = new List<string>();
            bool result = true;

            if (expectedWindowsReported != null)
            {
                if (!TestCaseHelpers.VerifyPropertiesPresent(Constants.JsonDeviceTwin, expectedWindowsReported, reportedProperties, errorList))
                {
                    logger.Log(LogLevel.Verbose, "---- Final Result:");
                    logger.Log(LogLevel.Verbose, reportedProperties.ToString());
                    logger.Log(LogLevel.Verbose, "---- Expected Present Result:");
                    logger.Log(LogLevel.Verbose, expectedWindowsReported.ToString());
                    result = false;
                }
            }

            if (_expectedAbsentReportedState != null)
            {
                JObject expectedAbsentReported = (JObject)_expectedAbsentReportedState[Constants.JsonPropertiesRoot][Constants.JsonReportedRoot];
                if (expectedAbsentReported != null)
                {
                    if (!TestCaseHelpers.VerifyPropertiesAbsent(expectedAbsentReported, reportedProperties, errorList))
                    {
                        logger.Log(LogLevel.Verbose, "---- Final Result:");
                        logger.Log(LogLevel.Verbose, reportedProperties.ToString());
                        logger.Log(LogLevel.Verbose, "---- Expected Absent Result:");
                        logger.Log(LogLevel.Verbose, expectedAbsentReported.ToString());
                        result = false;
                    }
                }
            }

            ReportResult(logger, result, errorList);
        }

        protected JObject _desiredState;
        protected JObject _expectedPresentReportedState;
        protected JObject _expectedAbsentReportedState;
    }
}
