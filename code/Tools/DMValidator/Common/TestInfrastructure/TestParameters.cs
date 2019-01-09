// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System.Collections.Generic;

namespace DMValidator
{
    class TestParameters
    {
        public string IoTHubConnectionString { get; set; }
        public string IoTHubDeviceId { get; set; }

        private void ResolveParametersInternal(JToken root)
        {
            if (root is JObject)
            {
                JObject jObject = (JObject)root;
                foreach (JProperty childProperty in jObject.Children())
                {
                    ResolveParametersInternal(childProperty);
                }
            }

            if (root is JProperty)
            {
                JProperty jProperty = (JProperty)root;
                if (jProperty.Value is JValue && jProperty.Value.Type == JTokenType.String)
                {
                    jProperty.Value = new JValue((string)jProperty.Value);
                }
                else if (jProperty.Value is JObject)
                {
                    ResolveParametersInternal(jProperty.Value);
                }
            }
        }

        public JToken ResolveParameters(JToken root)
        {
            JToken resolvedTree = root.DeepClone();
            ResolveParametersInternal(resolvedTree);
            return resolvedTree;
        }
    }
}
