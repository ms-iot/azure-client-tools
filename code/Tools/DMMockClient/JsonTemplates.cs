// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Text;

namespace DMMockClient
{
    static class JsonTemplates
    {
        public static readonly string ReportedProperties;

        static JsonTemplates()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append("{ \n");
            sb.Append("  \"rebootInfo\": {\n");
            sb.Append("    \"singleRebootTime\": \"2018-10-10T17:00:00Z\",\n");
            sb.Append("    \"dailyRebootTime\": \"2018-10-10T17:00:00Z\",\n");
            sb.Append("  }\n");
            sb.Append("}\n");
            ReportedProperties = sb.ToString();
        }
    }
}
