// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Windows;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;

namespace DMValidator
{
    abstract class TestCase
    {
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

        protected static void ReportError(ILogger logger, string msg)
        {
            logger.Log(LogLevel.Error, msg);
        }

        protected void ReportResult(ILogger logger, bool result, IEnumerable<string> errorList)
        {
            if (result)
            {
                logger.Log(LogLevel.Information, "Test " + _name + " succeeded!");
            }
            else
            {
                logger.Log(LogLevel.Error, "Test " + _name + " Failed!");
                foreach (string e in errorList)
                {
                    logger.Log(LogLevel.Error, e);
                }
            }
        }

        public abstract Task<bool> Execute(ILogger logger, IoTHubManager client, TestParameters testParameters);

        // Common properties...
        protected string _name;
        protected string _description;
    }
}
