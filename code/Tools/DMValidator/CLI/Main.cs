// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.Azure.Devices;
using NDesk.Options;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System;

namespace DMValidator
{
    class Program
    {
        static void Main(string[] args)
        {
            OptionManager optionMgr = new OptionManager();

            try
            {
                optionMgr.ComputeOptions(args);

                if(optionMgr._connectionString == null)
                {
                    throw new OptionException(OptionManager._connectionStringArgumentName +
                                              " argument is required!",
                                              OptionManager._connectionStringArgumentName);
                }
            }
            catch(OptionException e)
            {
                Console.Write("DMValidator: ");
                Console.WriteLine(e.Message);
                Console.WriteLine("Showing help prompt...");
                Console.WriteLine();
                optionMgr.ShowHelp();
            }

            TestCaseRunner runner = new TestCaseRunner(optionMgr._logPath, optionMgr._connectionString);

            //Get list of devices from IoT Hub
            runner.GetDevices();

            //If user just needs to list the devices, do that and quit.
            if(optionMgr._list)
            {
                foreach(string deviceId in runner._devicesToTest)
                {
                    Console.WriteLine(deviceId);
                }
                System.Environment.Exit(0);
            }

            //If user provided devices he wants to test, parse them and filter to only leave those devices
            if(optionMgr._devices != null)
            {
                runner.FilterDevices(optionMgr._devices);
                ReportUnusedDevices(optionMgr._devices, runner._devicesToTest);
            }

            //Get list of the JSON files
            runner.LoadScenarios(optionMgr._scenarios);

            //Run tests. Report to console.

            runner.ExecuteTestCases();
        }

        private static void ReportUnusedDevices(string[] requestedDevices, List<string> devicesToTest)
        {
            //See if we are missing any devices
            var missingDevices = requestedDevices.Except(devicesToTest);
            if(missingDevices.Count() != 0)
            {
                //We are. Tell the user which ones.
                Console.WriteLine("Some of the devices could not be found in IoT Hub!");
                Console.WriteLine("Missing devices:");
                foreach(string device in missingDevices)
                {
                    Console.WriteLine(device);
                }
                //If none of the devices could be found, throw early.
                if(missingDevices.Count() == requestedDevices.Count())
                {
                    throw new InvalidOperationException("None of the devices to test can be found!");
                }
            }
        }
    }

    class OptionManager
    {
        private const string DefaultConfigPath = "./DMValidator.json";
        public const string _connectionStringArgumentName = "CONNECTION_STRING";
        private const char DeviceSeparator = ',';
        private const string DefaultLogPath = "./";
        private const string ConnectionStringArgumentName = "connection-string";
        private const string DevicesArgumentName = "devices";
        private const string TestsArgumentName = "tests";
        private const string LogArgumentName = "log";
        private const string ListArgumentName = "list";
        private const string ConfigArgumentName = "CONFIG";
        private const string NoConfigArgumentName = "no-config";

        private OptionSet _options;
        public string _connectionString {get; private set;} = null;
        public string[] _devices {get; private set;} = null;
        public string _scenarios {get; private set;} = null;
        public string _logPath {get; private set;} = DefaultLogPath;
        public bool _list {get; private set;} = false;
        private string _configPath = DefaultConfigPath;
        private bool _useConfig = true;

        public void ComputeOptions(string[] args)
        {
            _options = new OptionSet() {
                {
                    $"c|{ConnectionStringArgumentName}=",
                    "REQUIRED: The Azure IoT Hub {" + _connectionStringArgumentName + "}.",
                    arg => _connectionString = arg
                },
                {
                    $"d|{DevicesArgumentName}=",
                    "{DEVICEID} of one or more devices to be tested, separated by \"" +
                    DeviceSeparator + "\" character. " +
                    "By default, all the IoT Hub devices are tested.",
                    arg => _devices = arg.Split(DeviceSeparator)
                },
                {
                    $"t|{TestsArgumentName}=",
                    "Path to one or more {TESTS} that are going to be executed. " +
                    "By default, all tests in '../TestCases/' are executed.",
                    arg => _scenarios = arg
                },
                {
                    $"L|{LogArgumentName}=",
                    "{DIRECTORY} where log file will be stored. " +
                    "By default, the current working directory will be used.",
                    arg => _logPath = arg
                },
                {
                    $"l|{ListArgumentName}",
                    "List the devices instead of running the tests.",
                    _ => _list = true
                },
                {
                    $"C|{ConfigArgumentName}=",
                    "Use a configuration {FILE}. Default location is \"" + DefaultConfigPath
                    + "\". no-config option has priority over this one.",
                    arg => _configPath = arg
                },
                {
                    $"n|{NoConfigArgumentName}",
                    "Do not use configuration file. This option has a priority over config option.",
                    _ => _useConfig = false
                },
                {
                    "h|help",
                    "Show this message and exit the program.",
                    _ => ShowHelp()
                },
            };

            _options.Parse(args);

            // User did not turn the config off
            if(_useConfig)
            {
                if(File.Exists(_configPath))
                {
                    JObject config = JObject.Parse(File.ReadAllText(_configPath));

                    if(null == _connectionString)
                    {
                        var value = config[ConnectionStringArgumentName];
                        if(null != value)
                        {
                            _connectionString = (string)value;
                        }
                    }

                    if(null == _devices)
                    {
                        var value = config[DevicesArgumentName];
                        if(null != value)
                        {
                            _devices = ((JArray)value).Select(jv => (string)jv).ToArray();
                        }
                    }

                    if(null == _scenarios)
                    {
                        var value = config[TestsArgumentName];
                        if(null != value)
                        {
                            _scenarios = (string)value;
                        }

                    }

                    if(DefaultLogPath == _logPath)
                    {
                        var value = config[LogArgumentName];
                        if(null != value)
                        {
                            _logPath = (string)value;
                        }
                    }
                }
                else
                {
                    Console.WriteLine("No config file found at " + _configPath + " - skipping loading config file!");
                }
            }
        }

        public void ShowHelp()
        {
            Console.WriteLine("Usage: dmvalidator [options]");
            Console.WriteLine("Run tests on selected devices connected to the Azure IoT Hub.");
            Console.WriteLine("Options: ");
            _options.WriteOptionDescriptions(Console.Out);
            System.Environment.Exit(2);
        }
    }

    class TestCaseRunner
    {

        private const string QueryDeviceId = "DeviceId";
        private const string QueryDeviceList = "SELECT deviceId from Devices";
        private const string RegistryConnectionString = "ConnectionString";
        private const string RegistryStore = "Software\\Microsoft\\DMValidator";
        private const string DefaultScenariosPath = @"./TestCases/";

        private Logger _logger;
        public List<string> _devicesToTest {get; private set;} = new List<string>();
        public List<string> _testScenarios = new List<string>();
        public string _IoTHubConnectionString;

        public TestCaseRunner(string logFilePath, string connectionString)
        {
            _logger = new Logger();
            _logger.TargetLogPath = logFilePath;
            _IoTHubConnectionString = connectionString;
        }

        public void GetDevices()
        {
            GetDevicesListAsync().GetAwaiter().GetResult();
        }

        private async Task GetDevicesListAsync()
        {
            RegistryManager registryManager = RegistryManager.CreateFromConnectionString(_IoTHubConnectionString);

            IQuery query = registryManager.CreateQuery(QueryDeviceList);
            IEnumerable<string> results = await query.GetNextAsJsonAsync();

            foreach (string s in results)
            {
                JObject jObject = (JObject)JsonConvert.DeserializeObject(s);
                _devicesToTest.Add((string)jObject[QueryDeviceId]);
            }

            _devicesToTest.Sort();
        }

        public void FilterDevices(string[] allowed)
        {
            _devicesToTest = _devicesToTest.Intersect(allowed).ToList();
        }

        private async Task ExecuteTestCasesAsync()
        {
            if(!_devicesToTest.Any())
            {
                throw new InvalidOperationException("List of the devices to be tested is empty!");
            }

            foreach(string deviceId in _devicesToTest)
            {
                TestParameters testParameters = new TestParameters();
                testParameters.IoTHubConnectionString = _IoTHubConnectionString;
                testParameters.IoTHubDeviceId = deviceId;

                List<string> summaryList = new List<string>();

                bool allResult = true;
                foreach (string item in _testScenarios)
                {
                    string scenarioFileName = item;
                    _logger.Log(LogLevel.Information, "Processing " + scenarioFileName);
                    Console.WriteLine("Processing " + item);

                    bool result = await TestScenario.Run(_logger, scenarioFileName, testParameters);

                    summaryList.Add((result ? "[ok] " : "[xx] ") + scenarioFileName);
                    allResult &= result;
                }

                _logger.Log(LogLevel.Information, "---- Summary ------------------------------------------------------------------");
                foreach (string s in summaryList)
                {
                    _logger.Log(LogLevel.Information, s);
                }

                _logger.Log(LogLevel.Information, "---> " + (allResult ? "All succeeded" : "One ore more tests have failed"));
            }
        }

        public void ExecuteTestCases()
        {
            _logger.CreateNewFile();
            ExecuteTestCasesAsync().GetAwaiter().GetResult();
        }

        public void LoadScenarios(string path)
        {
            if(path == null)
            {
                path = DefaultScenariosPath;
            }

            foreach (var f in Directory.EnumerateFiles(path, "*.json", SearchOption.AllDirectories))
            {
                _testScenarios.Add(f);
            }
        }

        public void PrintScenarios()
        {
            foreach(string scenario in _testScenarios)
            {
                Console.WriteLine(scenario);
            }
        }
    }
}
