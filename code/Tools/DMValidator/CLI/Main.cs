// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using Microsoft.Azure.Devices;
using NDesk.Options;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace DMValidator
{
    class Program
    {
        private const string QueryDeviceId = "DeviceId";
        private const string QueryDeviceList = "SELECT deviceId from Devices";

        static void Main(string[] args)
        {
            OptionManager optionMgr = new OptionManager();

            try
            {
                optionMgr.ComputeOptions(args);

                if (optionMgr._connectionString == null)
                {
                    throw new OptionException(OptionManager._connectionStringArgumentName +
                                              " argument is required!",
                                              OptionManager._connectionStringArgumentName);
                }

                // Initialize the log...
                _logger = new Logger();
                _logger.TargetLogPath = optionMgr._logPath;
                _logger.CreateNewFile();
                _logger.Log(LogLevel.Information, "Log file is: " + _logger.GetLogFileName());

                // Initialize cloud service access...
                CloudServices cloudServices = new CloudServices(optionMgr._connectionString, optionMgr._storageConnectionString);

                // Initialize the test running...
                TestCaseRunner runner = new TestCaseRunner(
                    _logger,
                    cloudServices);

                // Set target devices...
                runner.DevicesToTest = GetTestDevicesList(optionMgr, cloudServices);

                // Set target tests
                runner.LoadFolderScenarios(optionMgr._scenariosFolder);
                runner.LoadFileScenarios(optionMgr._scenarioFiles);

                // Run the tests...
                runner.Execute();

                Environment.Exit(0);
            }
            catch (OptionException e)
            {
                Console.Write("DMValidator: ");
                Console.WriteLine(e.Message);
                Console.WriteLine("Showing help prompt...");
                Console.WriteLine();
                optionMgr.ShowHelp();

                Environment.Exit(-1);
            }
            catch (Exception e)
            {
                if (_logger != null)
                {
                    _logger.Log(LogLevel.Error, e.Message);
                }
                else
                {
                    Console.WriteLine("Error", e.Message);
                }

                Environment.Exit(-1);
            }
        }

        static private List<string> GetTestDevicesList(OptionManager optionMgr, CloudServices cloudServices)
        {
            // Get list of devices from IoT Hub
            List<string> deviceList = GetDevicesListAsync(cloudServices.IoTRegistryManager).GetAwaiter().GetResult(); ;

            // If user just needs to list the devices, do that and quit.
            if (optionMgr._list)
            {
                foreach (string deviceId in deviceList)
                {
                    Console.WriteLine(deviceId);
                }
                System.Environment.Exit(0);
            }

            List<string> testDeviceList = new List<string>();

            // If user provided devices he wants to test, parse them and filter to only leave those devices
            if (optionMgr._devices != null)
            {
                testDeviceList = deviceList.Intersect(optionMgr._devices).ToList();
                ReportNotFoundDevices(optionMgr._devices /*required*/, testDeviceList /*found*/);
            }

            return testDeviceList;
        }

        static private async Task<List<string>> GetDevicesListAsync(RegistryManager registryManager)
        {
            IQuery query = registryManager.CreateQuery(QueryDeviceList);
            IEnumerable<string> results = await query.GetNextAsJsonAsync();

            List<string> deviceList = new List<string>();
            foreach (string s in results)
            {
                JObject jObject = (JObject)JsonConvert.DeserializeObject(s);
                deviceList.Add((string)jObject[QueryDeviceId]);
            }
            deviceList.Sort();

            return deviceList;
        }

        static private void ReportNotFoundDevices(string[] requestedDevices, List<string> devicesToTest)
        {
            // See if we are missing any devices
            var missingDevices = requestedDevices.Except(devicesToTest);
            if (missingDevices.Count() != 0)
            {
                // We are. Tell the user which ones.
                Console.WriteLine("Some of the devices could not be found in IoT Hub!");
                Console.WriteLine("Missing devices:");
                foreach (string device in missingDevices)
                {
                    Console.WriteLine(device);
                }
                // If none of the devices could be found, throw early.
                if (missingDevices.Count() == requestedDevices.Count())
                {
                    throw new InvalidOperationException("None of the devices to test can be found!");
                }
            }
        }

        static private Logger _logger;
    }
}
