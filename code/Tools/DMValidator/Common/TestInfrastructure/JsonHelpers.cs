// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;

namespace DMValidator
{
    static class JsonHelpers
    {
        public static bool TryGetString(JObject obj, string propertyName, out string propertyValue)
        {
            propertyValue = "";

            if (obj == null)
            {
                return false;
            }

            JToken token = obj[propertyName];
            if (token is JValue)
            {
                JValue jValue = (JValue)token;
                if (jValue.Type == JTokenType.String)
                {
                    propertyValue = (string)jValue;
                    return true;
                }
            }
            return false;
        }

        public static bool TryGetObject(JObject obj, string propertyName, out JObject propertyValue)
        {
            propertyValue = null;

            if (obj == null)
            {
                return false;
            }

            JToken token = obj[propertyName];
            if (token is JObject)
            {
                propertyValue = (JObject)token;
                return true;
            }
            return false;
        }

        public static bool TryGetArray(JObject obj, string propertyName, out JArray propertyValue)
        {
            propertyValue = null;

            if (obj == null)
            {
                return false;
            }

            JToken token = obj[propertyName];
            if (token is JArray)
            {
                propertyValue = (JArray)token;
                return true;
            }
            return false;
        }

        public static bool TryGetValue(JObject obj, string propertyName, out JValue propertyValue)
        {
            propertyValue = null;

            if (obj == null)
            {
                return false;
            }

            JToken token = obj[propertyName];
            if (token is JValue)
            {
                propertyValue = (JValue)token;
                return true;
            }
            return false;
        }
    }
}
