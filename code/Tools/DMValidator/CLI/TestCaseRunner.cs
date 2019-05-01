// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace DMValidator
{
    class TestCaseRunner
    {
        public List<string> DevicesToTest { get; set; }

        public TestCaseRunner(Logger logger, CloudServices cloudServices)
        {
            _logger = logger;
            _cloudServices = cloudServices;
        }

        public void LoadFolderScenarios(string scenariosFolder)
        {
            if (String.IsNullOrWhiteSpace(scenariosFolder))
            {
                _logger.Log(LogLevel.Information, "No scenarios folder is specified.");
                return;
            }

            _logger.Log(LogLevel.Information, "Adding scenario files under: " + scenariosFolder);
            foreach (var f in Directory.EnumerateFiles(scenariosFolder, "*.json", SearchOption.AllDirectories))
            {
                _logger.Log(LogLevel.Information, "    Adding: " + f);
                _testScenarios.Add(f);
            }
        }

        public void LoadFileScenarios(string[] scenarioFiles)
        {
            if (scenarioFiles == null)
            {
                _logger.Log(LogLevel.Information, "No scenario files are specified explicitly.");
                return;
            }

            _logger.Log(LogLevel.Information, "Adding explicitly specified scenario files...");
            foreach (var f in scenarioFiles)
            {
                _logger.Log(LogLevel.Information, "    Adding: " + f);
                _testScenarios.Add(f);
            }
        }

        private async Task DeviceExecuteAsync(string deviceId)
        {
            TestParameters testParameters = new TestParameters();
            testParameters.IoTCloudServices = _cloudServices;
            testParameters.IoTHubDeviceId = deviceId;

            List<string> summaryList = new List<string>();

            bool allResult = true;
            foreach (string scenarioFileName in _testScenarios)
            {
                bool result = false;
                try
                {
                    await TestScenario.Run(_logger, scenarioFileName, testParameters);
                    result = true;
                }
                catch (Exception e)
                {
                    _logger.Log(LogLevel.Error, e.Message);
                }
                summaryList.Add((result ? "[ok] " : "[xx] ") + scenarioFileName);
                allResult &= result;
            }

            _logger.Log(LogLevel.Information, "---- Device `" + deviceId + "` Summary ------------------------------------------------------------------");
            foreach (string s in summaryList)
            {
                _logger.Log(LogLevel.Information, s);
            }

            if (allResult)
            {
                _logger.Log(LogLevel.Information, "---> All scenarios succeeded for device: " + deviceId);
            }
            else
            {
                throw new Exception("On or more tests failed for device: " + deviceId);
            }
        }

        private async Task ExecuteAsync()
        {
            if (DevicesToTest.Count == 0)
            {
                throw new InvalidOperationException("List of the devices to be tested cannot be empty!");
            }

            bool allResult = true;

            foreach (string deviceId in DevicesToTest)
            {
                _logger.Log(LogLevel.Information, "Current Device: " + deviceId);

                bool result = false;
                try
                {
                    await DeviceExecuteAsync(deviceId);
                    result = true;
                }
                catch (Exception e)
                {
                    _logger.Log(LogLevel.Error, e.Message);
                }

                allResult &= result;
            }

            if (allResult)
            {
                _logger.Log(LogLevel.Information, "---> All devices succeeded.");
            }
            else
            {
                throw new InvalidOperationException("One or more devices had test failures!");
            }
        }

        public void Execute()
        {
            _logger.Log(LogLevel.Information, "Current Directory: " + Directory.GetCurrentDirectory());

            ExecuteAsync().GetAwaiter().GetResult();
        }

        // Data Members

        private Logger _logger;
        private CloudServices _cloudServices;
        private List<string> _testScenarios = new List<string>();
    }
}