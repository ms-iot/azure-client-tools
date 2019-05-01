// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Copyright (c) Microsoft Corporation. All rights reserved.

// Licensed under the MIT License.

using System;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace DMValidator
{
    enum LogLevel
    {
        Verbose,
        Information,
        Warning,
        Error,
        Critical
    }

    interface ILogger
    {
        void Log(LogLevel logLevel, string message);
    }

    class Logger : ILogger
    {
        public string TargetLogPath
        {
            get
            {
                return _targetLogPath;
            }

            set
            {
                if (_targetLogPath == value)
                {
                    return;
                }

                _targetLogPath = value;
                CreateNewFile();
            }
        }

        public void CreateNewFile()
        {
            Directory.CreateDirectory(_targetLogPath);

            _targetLogFile = _targetLogPath + "/" + GenerateLogFileName();
            using (StreamWriter sw = File.CreateText(_targetLogFile))
            {
            }
        }

        public Logger()
        {
        }

        private static string GenerateLogFileName()
        {
            return "DMValidator_" + DateTime.Now.ToString("yyyy_MM_dd_HH_mm_ss") + ".log";
        }

        public string GetLogFileName()
        {
            return _targetLogFile;
        }

        public void Log(LogLevel logLevel, string message)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(DateTime.Now.ToString("HH:mm:ss"));
            sb.Append(": " + logLevel.ToString().PadRight(12) + ": ");
            sb.Append(message);

            string formattedMessage = sb.ToString();

            // Debug window...
            Debug.WriteLine(formattedMessage);

            // Console window...
            Console.WriteLine(formattedMessage);

            // Text file...
            if (_targetLogPath != null)
            {
                using (StreamWriter sw = File.AppendText(_targetLogFile))
                {
                    sw.WriteLine(formattedMessage);
                }
            }
        }

        string _targetLogPath;
        string _targetLogFile;
    }
}
