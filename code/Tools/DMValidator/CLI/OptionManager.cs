// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NDesk.Options;
using Newtonsoft.Json.Linq;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System;

namespace DMValidator
{
    class OptionManager
    {
        private const string DefaultConfigPath = "./DMValidator.json";
        public const string _connectionStringArgumentName = "CONNECTION_STRING";
        private const char DeviceSeparator = ',';
        private const string DefaultLogPath = "./";
        private const string ConnectionStringArgumentName = "connection-string";
        private const string StorageConnectionStringArgumentName = "storage-connection-string";
        private const string DevicesArgumentName = "devices";
        private const string ScenariosFolderArgumentName = "scenariosFolder";
        private const string ScenarioFilesArgumentName = "scenarioFiles";
        private const string LogArgumentName = "log";
        private const string ListArgumentName = "list";
        private const string ConfigArgumentName = "CONFIG";
        private const string NoConfigArgumentName = "no-config";

        private OptionSet _options;
        public string _connectionString { get; private set; } = null;
        public string _storageConnectionString { get; private set; } = null;
        public string[] _devices { get; private set; } = null;
        public string _scenariosFolder { get; private set; } = null;
        public string [] _scenarioFiles { get; private set; } = null;
        public string _logPath { get; private set; } = DefaultLogPath;
        public bool _list { get; private set; } = false;
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
                    $"s|{StorageConnectionStringArgumentName}=",
                    "REQUIRED: The Azure Storage {" + _connectionStringArgumentName + "}.",
                    arg => _storageConnectionString = arg
                },
                {
                    $"d|{DevicesArgumentName}=",
                    "{DEVICEID} of one or more devices to be tested, separated by \"" +
                    DeviceSeparator + "\" character. " +
                    "By default, all the IoT Hub devices are tested.",
                    arg => _devices = arg.Split(DeviceSeparator)
                },
                {
                    $"t|{ScenariosFolderArgumentName}=",
                    "Path to one or more {TESTS} that are going to be executed. " +
                    "By default, all tests in '../TestCases/' are executed.",
                    arg => _scenariosFolder = arg
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
            if (_useConfig)
            {
                ReadConfigFile();
            }
        }

        private void ReadConfigFile()
        {
            Console.WriteLine("Reading config file: " + _configPath);

            if (!File.Exists(_configPath))
            {
                Console.WriteLine("No config file found at " + _configPath + " - skipping loading config file!");
                return;
            }

            JObject config = JObject.Parse(File.ReadAllText(_configPath));

            if (null == _connectionString)
            {
                var value = config[ConnectionStringArgumentName];
                if (null != value)
                {
                    _connectionString = (string)value;
                }
            }

            if (null == _storageConnectionString)
            {
                var value = config[StorageConnectionStringArgumentName];
                if (null != value)
                {
                    _storageConnectionString = (string)value;
                }
            }

            if (null == _devices)
            {
                var value = config[DevicesArgumentName];
                if (null != value)
                {
                    _devices = ((JArray)value).Select(jv => (string)jv).ToArray();
                }
            }

            if (null == _scenariosFolder)
            {
                var value = config[ScenariosFolderArgumentName];
                if (null != value)
                {
                    _scenariosFolder = (string)value;
                }
            }

            if (null == _scenarioFiles)
            {
                var value = config[ScenarioFilesArgumentName];
                if (null != value)
                {
                    if (value.Type != JTokenType.Array)
                    {
                        Console.WriteLine("Error: Invalid value type for " + ScenarioFilesArgumentName);
                    }

                    List<string> valuesList = new List<string>();
                        
                    foreach (var v in (JArray)value)
                    {
                        valuesList.Add((string)v);
                    }
                    _scenarioFiles = valuesList.ToArray();
                }
            }

            if (DefaultLogPath == _logPath)
            {
                var value = config[LogArgumentName];
                if (null != value)
                {
                    _logPath = (string)value;
                }
            }
        }

        public void ShowHelp()
        {
            Console.WriteLine("Usage: dmvalidator [options]");
            Console.WriteLine("Run tests on selected devices connected to the Azure IoT Hub.");
            Console.WriteLine("Options: ");
            _options.WriteOptionDescriptions(Console.Out);
        }
    }
}