// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.IO;

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

        public static string GetString(JObject obj, string propertyName)
        {
            string propertyValue;
            if (!TryGetString(obj, propertyName, out propertyValue))
            {
                throw new Exception("Unable to find expected string property `" + propertyName + "`");
            }
            return propertyValue;
        }

        public static bool TryGetInt(JObject obj, string propertyName, out int propertyValue)
        {
            propertyValue = 0;

            if (obj == null)
            {
                return false;
            }

            JToken token = obj[propertyName];
            if (token is JValue)
            {
                JValue jValue = (JValue)token;
                if (jValue.Type == JTokenType.Integer)
                {
                    propertyValue = (int)jValue;
                    return true;
                }
            }
            return false;
        }

        public static int GetInt(JObject obj, string propertyName)
        {
            int propertyValue;
            if (!TryGetInt(obj, propertyName, out propertyValue))
            {
                throw new Exception("Unable to find expected int property `" + propertyName + "`");
            }
            return propertyValue;
        }

        public static bool TryGetLong(JObject obj, string propertyName, out long propertyValue)
        {
            propertyValue = 0;

            if (obj == null)
            {
                return false;
            }

            JToken token = obj[propertyName];
            if (token is JValue)
            {
                JValue jValue = (JValue)token;
                if (jValue.Type == JTokenType.Integer)
                {
                    propertyValue = (long)jValue;
                    return true;
                }
            }
            return false;
        }

        public static bool TryGetDateTime(JObject obj, string propertyName, out DateTime propertyValue)
        {
            propertyValue = new DateTime();

            if (obj == null)
            {
                return false;
            }

            JToken token = obj[propertyName];
            if (token is JValue)
            {
                JValue jValue = (JValue)token;
                if (jValue.Type == JTokenType.Date)
                {
                    propertyValue = (DateTime)jValue;
                    return true;
                }
            }
            return false;
        }

        public static bool TryGetString(JObject obj, string propertyName, out bool ignore, out string propertyValue)
        {
            propertyValue = "";
            ignore = false;

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
                    if ((string)jValue == "<ignore>")
                    {
                        ignore = true;
                    }
                    else
                    {
                        propertyValue = (string)jValue;
                    }
                    return true;
                }
            }
            return false;
        }

        public static string GetString(JObject obj, string propertyName, out bool ignore)
        {
            string propertyValue;
            if (!TryGetString(obj, propertyName, out ignore, out propertyValue))
            {
                throw new Exception("Unable to find expected string property `" + propertyName + "`");
            }
            return propertyValue;
        }

        public static bool TryGetInt(JObject obj, string propertyName, out bool ignore, out int propertyValue)
        {
            propertyValue = 0;
            ignore = false;

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
                    if ((string)jValue == "<ignore>")
                    {
                        ignore = true;
                        return true;
                    }
                }
                if (jValue.Type == JTokenType.Integer)
                {
                    propertyValue = (int)jValue;
                    return true;
                }
            }
            return false;
        }

        public static int GetInt(JObject obj, string propertyName, out bool ignore)
        {
            int propertyValue;
            if (!TryGetInt(obj, propertyName, out ignore, out propertyValue))
            {
                throw new Exception("Unable to find expected int property `" + propertyName + "`");
            }
            return propertyValue;
        }

        public static bool TryGetLong(JObject obj, string propertyName, out bool ignore, out long propertyValue)
        {
            propertyValue = 0;
            ignore = false;

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
                    if ((string)jValue == "<ignore>")
                    {
                        ignore = true;
                        return true;
                    }
                }
                if (jValue.Type == JTokenType.Integer)
                {
                    propertyValue = (long)jValue;
                    return true;
                }
            }
            return false;
        }

        public static long GetLong(JObject obj, string propertyName, out bool ignore)
        {
            long propertyValue;
            if (!TryGetLong(obj, propertyName, out ignore, out propertyValue))
            {
                throw new Exception("Unable to find expected long property `" + propertyName + "`");
            }
            return propertyValue;
        }

        public static bool TryGetDateTime(JObject obj, string propertyName, out bool ignore, out DateTime propertyValue)
        {
            propertyValue = new DateTime();
            ignore = false;

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
                    if ((string)jValue == "<ignore>")
                    {
                        ignore = true;
                        return true;
                    }
                }
                if (jValue.Type == JTokenType.Date)
                {
                    propertyValue = (DateTime)jValue;
                    return true;
                }
            }
            return false;
        }

        public static DateTime GetDateTime(JObject obj, string propertyName, out bool ignore)
        {
            DateTime propertyValue;
            if (!TryGetDateTime(obj, propertyName, out ignore, out propertyValue))
            {
                throw new Exception("Unable to find expected date-time property `" + propertyName + "`");
            }
            return propertyValue;
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

        public static JObject GetObject(JObject obj, string propertyName)
        {
            JObject propertyValue;
            if (!TryGetObject(obj, propertyName, out propertyValue))
            {
                throw new Exception("Unable to find expected json object property `" + propertyName + "`");
            }
            return propertyValue;
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

        public static JArray GetArray(JObject obj, string propertyName)
        {
            JArray propertyValue;
            if (!TryGetArray(obj, propertyName, out propertyValue))
            {
                throw new Exception("Unable to find expected array property `" + propertyName + "`");
            }
            return propertyValue;
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

        public static bool TryGetToken(JObject obj, string propertyName, out JToken propertyValue)
        {
            propertyValue = null;

            if (obj == null)
            {
                return false;
            }

            JToken token = obj[propertyName];
            if (token != null)
            {
                propertyValue = token;
                return true;
            }
            return false;
        }

        public static JObject JsonObjectFromFile(ILogger logger, string fileName)
        {
            string jsonString = File.ReadAllText(fileName);
            object deserializedObject = JsonConvert.DeserializeObject(jsonString);
            return (JObject)deserializedObject;
        }
    }
}
