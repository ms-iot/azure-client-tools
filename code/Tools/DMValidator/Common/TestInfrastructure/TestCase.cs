// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace DMValidator
{
    abstract class TestCase
    {
        protected const string Indent = "       ";
        protected const string IndentInner = "         ";

        public string Name
        {
            get
            {
                return _name;
            }
        }

        public virtual void Dump()
        {
        }

        protected static void FromJson(ILogger logger, JObject testCaseJson, TestCase testCase)
        {
            testCase._name = JsonHelpers.GetString(testCaseJson, Constants.TCJsonName);
            testCase._description = JsonHelpers.GetString(testCaseJson, Constants.TCJsonDescription);

            if (!JsonHelpers.TryGetInt(testCaseJson, Constants.TCJsonReadBackPause, out testCase._readBackPause))
            {
                testCase._readBackPause = Constants.TCJsonReadBackPauseDefault;
            }
        }

        protected void LogMismatch(ILogger logger, string expected, string actual)
        {
            logger.Log(LogLevel.Verbose, IndentInner + "-- Error Mismatch");
            logger.Log(LogLevel.Verbose, IndentInner + "---- Expected:");
            logger.Log(LogLevel.Verbose, IndentInner + expected);
            logger.Log(LogLevel.Verbose, IndentInner + "---- Actual:");
            logger.Log(LogLevel.Verbose, IndentInner + actual);
        }

        protected static void ReportError(ILogger logger, string msg)
        {
            logger.Log(LogLevel.Error, msg);
        }

        protected void ReportResult(ILogger logger, bool result, IEnumerable<string> errorList)
        {
            if (!result)
            {
                logger.Log(LogLevel.Error, IndentInner + "Failed");
                foreach (string e in errorList)
                {
                    logger.Log(LogLevel.Error, e);
                }
                throw new System.Exception(IndentInner + "Test Case `" + _name + "` failed.");
            }
        }

        protected void ReportResult(ILogger logger, bool result, string error)
        {
            if (!result)
            {
                logger.Log(LogLevel.Error, IndentInner + "Failed");
                logger.Log(LogLevel.Error, error);
                throw new System.Exception(IndentInner + "Test Case `" + _name + "` failed.");
            }
        }

        public abstract Task Execute(ILogger logger, TestParameters testParameters);

        // Common properties...
        protected string _name;
        protected string _description;
        protected int _readBackPause;
    }
}
