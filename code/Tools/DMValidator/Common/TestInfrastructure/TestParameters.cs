// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System.Collections.Generic;

namespace DMValidator
{
    class TestParameters
    {
        private const string JsonAzureStorageConnectionStringHint = "<storage connection string>";

        public TestParameters()
        {
            _dictionary = new Dictionary<string, string>();
        }

        public CloudServices IoTCloudServices
        {
            get
            {
                return _cloudServices;
            }
            set
            {
                _cloudServices = value;
                if (_cloudServices != null)
                {
                    _dictionary[JsonAzureStorageConnectionStringHint] = _cloudServices.AzureStorageConnectionString;
                }
            }
        }

        public string IoTHubDeviceId { get; set; }

        private string Resolve(string key)
        {
            string value;
            if (_dictionary.TryGetValue(key, out value))
            {
                return value;
            }
            return key;
        }

        // These 'Resolve...' functions are just placeholders from V1.
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
                    jProperty.Value = new JValue(Resolve((string)jProperty.Value));
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

        private Dictionary<string, string> _dictionary;
        private CloudServices _cloudServices;
    }
}
