// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace DMValidator
{
    class TestScenario
    {
        private TestScenario()
        {
            _testCases = new List<TestCase>();
        }

        public static async Task Run(ILogger logger, string scenarioFileName, TestParameters testParameters)
        {
            logger.Log(LogLevel.Information, "-- Running scenario: " + scenarioFileName);

            JObject scenarioJsonObject = JsonHelpers.JsonObjectFromFile(logger, scenarioFileName);

            TestScenario testScenario = ScenarioFromJson(logger, scenarioFileName, scenarioJsonObject);

            await testScenario.Execute(logger, testParameters);
        }

        private static TestCase TestCaseFromJson(ILogger logger, JObject testCaseJson)
        {
            string type;
            if (!JsonHelpers.TryGetString(testCaseJson, Constants.TCJsonType, out type))
            {
                throw new Exception("Test case json is missing required property `" + Constants.TCJsonType + "`.");
            }

            TestCase testCase = null;
            switch (type)
            {
                case Constants.TCJsonTypeInteractionDeviceTwin:
                    testCase = DeviceTwinTestCase.FromJson(logger, testCaseJson);
                    break;
                case Constants.TCJsonTypeInteractionDirectMethod:
                    testCase = DirectMethodTestCase.FromJson(logger, testCaseJson);
                    break;
                default:
                    throw new Exception("Test case json type is unknown `" + type + "`.");
            }

            return testCase;
        }

        private static List<TestCase> TestCasesFromJson(ILogger logger, JObject jObject, string propertyName)
        {
            JArray jsonTestCases = JsonHelpers.GetArray(jObject, propertyName);
            if (jsonTestCases.Count == 0)
            {
                throw new Exception("Scenario json definition must contain at least one test case definition.");
            }

            List<TestCase> testCases = new List<TestCase>();
            foreach (JToken testCaseToken in jsonTestCases)
            {
                if (!(testCaseToken is JObject))
                {
                    throw new Exception("Test case json definition must ber a json object.");
                }

                testCases.Add(TestCaseFromJson(logger, (JObject)testCaseToken));
            }
            return testCases;
        }

        private static TestScenario ScenarioFromJson(ILogger logger, string scenarioFileName, object deserializedContent)
        {
            logger.Log(LogLevel.Information, "     Parsing test cases...");

            if (deserializedContent == null || !(deserializedContent is JObject))
            {
                throw new Exception("Scenario json definition cannot be null or not a json object.");
            }

            TestScenario testScenario = new TestScenario();
            testScenario._scenarioFileName = scenarioFileName;
            testScenario._testCases = TestCasesFromJson(logger, (JObject)deserializedContent, Constants.JsonScenario);
            return testScenario;
        }

        private async Task ClearDeviceTwin(ILogger logger, IoTHubManager client, TestParameters testParameters)
        {
            logger.Log(LogLevel.Information, "       Clearing device: " + testParameters.IoTHubDeviceId);

            // Clean desired properties...
            // ToDo: need to read those properties from the scenario information.
            await client.UpdateDesiredProperty(testParameters.IoTHubDeviceId, Constants.JsonDeviceInfo, null);
            await client.UpdateDesiredProperty(testParameters.IoTHubDeviceId, Constants.JsonRebootInfo, null);
            await client.UpdateDesiredProperty(testParameters.IoTHubDeviceId, Constants.JsonTimeInfo, null);
            await client.UpdateDesiredProperty(testParameters.IoTHubDeviceId, Constants.JsonWindowsTelemetry, null);
            await client.UpdateDesiredProperty(testParameters.IoTHubDeviceId, Constants.JsonWindowsUpdate, null);

            // Clean reported properties...
            DeviceMethodReturnValue ret = await client.InvokeDirectMethod(testParameters.IoTHubDeviceId, Constants.JsonClearReportedCmd, Constants.JsonDirectMethodEmptyParams);
            if (ret.Status != IoTHubManager.DirectMethodSuccessCode)
            {
                throw new Exception("    XX Failed to clear the reported properties. Code: " + ret.Status);
            }

            logger.Log(LogLevel.Information, "       Cleared the device twin successfully");
        }

        private async Task Execute(ILogger logger, TestParameters testParameters)
        {
            logger.Log(LogLevel.Information, "     Executing test scenario...");

            await ClearDeviceTwin(logger, testParameters.IoTCloudServices.IoTHubManager, testParameters);

            foreach (TestCase testCase in _testCases)
            {
                logger.Log(LogLevel.Information, "       Executing test case " + testCase.Name + "...");

                await testCase.Execute(logger, testParameters);

                logger.Log(LogLevel.Information, "         ok.");
            }
        }

        // Data members...

        private string _scenarioFileName;
        private List<TestCase> _testCases;
    }
}
