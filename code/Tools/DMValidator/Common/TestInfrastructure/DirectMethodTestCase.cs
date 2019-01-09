// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;

namespace DMValidator
{
    class DirectMethodTestCase : TestCase
    {
        public override void Dump()
        {
            Debug.WriteLine("Name              : " + _name);
            Debug.WriteLine("Desription        : " + _description);
            Debug.WriteLine("Method Name             : " + _methodName);
            Debug.WriteLine("Parameters              : " + _parameters.ToString());
            if (_expectedPresentReportedState != null)
            {
                Debug.WriteLine("Expected Present Output: " + _expectedPresentReportedState.ToString());
            }
            else
            {
                Debug.WriteLine("Expected Present Output: null");
            }
            if (_expectedAbsentReportedState != null)
            {
                Debug.WriteLine("Expected Absent Output: " + _expectedAbsentReportedState.ToString());
            }
            else
            {
                Debug.WriteLine("Expected Absent Output: null");
            }
            Debug.WriteLine("Expected return json    : " + (_expectedReturnJson == null ? "null" : _expectedReturnJson.ToString()));
            Debug.WriteLine("Expected return code    : " + _expectedReturnCode);
        }

        public static DirectMethodTestCase FromJson(ILogger logger, JObject testCaseJson)
        {
            string name;
            if (!JsonHelpers.TryGetString(testCaseJson, Constants.TCJsonName, out name))
            {
                ReportError(logger, "Missing " + Constants.TCJsonName);
                return null;
            }

            string description;
            if (!JsonHelpers.TryGetString(testCaseJson, Constants.TCJsonDescription, out description))
            {
                ReportError(logger, "Missing " + Constants.TCJsonDescription);
                return null;
            }

            string methodName;
            if (!JsonHelpers.TryGetString(testCaseJson, Constants.JsonDirectMethodName, out methodName))
            {
                ReportError(logger, "Missing " + Constants.JsonDirectMethodName);
                return null;
            }

            JObject input;
            if (!JsonHelpers.TryGetObject(testCaseJson, Constants.TCJsonInput, out input))
            {
                ReportError(logger, "Missing " + Constants.TCJsonInput);
                return null;
            }

            JObject output;
            JObject expectedPresentReportedState = null;
            JObject expectedAbsentReportedState = null;
            JObject returnJson = null;
            JValue returnCode = null;
            if (!JsonHelpers.TryGetObject(testCaseJson, Constants.TCJsonOutput, out output))
            {
                ReportError(logger, "Missing " + Constants.TCJsonOutput);
                return null;
            }
            else
            {
                JObject deviceTwin = null;
                if (JsonHelpers.TryGetObject(output, Constants.TCJsonMethodDeviceTwin, out deviceTwin))
                {
                    if (JsonHelpers.TryGetObject(output, Constants.TCJsonOutputPresent, out expectedPresentReportedState))
                    {
                        expectedPresentReportedState = (JObject)expectedPresentReportedState.DeepClone();
                    }

                    if (JsonHelpers.TryGetObject(output, Constants.TCJsonOutputAbsent, out expectedAbsentReportedState))
                    {
                        expectedAbsentReportedState = (JObject)expectedAbsentReportedState.DeepClone();
                    }
                }
                JsonHelpers.TryGetObject(output, Constants.TCJsonMethodReturnJson, out returnJson);
                JsonHelpers.TryGetValue(output, Constants.TCJsonMethodReturnCode, out returnCode);
            }

            DirectMethodTestCase testCase = new DirectMethodTestCase();
            testCase._name = name;
            testCase._description = description;
            testCase._methodName = methodName;
            testCase._parameters = input;
            testCase._expectedPresentReportedState = expectedPresentReportedState;
            testCase._expectedAbsentReportedState = expectedAbsentReportedState;
            testCase._expectedReturnJson = returnJson;
            testCase._expectedReturnCode = (int)returnCode;
            return testCase;
        }

        public override Task<bool> Execute(ILogger logger, IoTHubManager client, TestParameters testParameters)
        {
            return Task.FromResult<bool>(true);
        }

        private string _methodName;
        private JObject _parameters;
        private JObject _expectedPresentReportedState;
        private JObject _expectedAbsentReportedState;
        private JObject _expectedReturnJson;
        private int _expectedReturnCode;
    }
}
