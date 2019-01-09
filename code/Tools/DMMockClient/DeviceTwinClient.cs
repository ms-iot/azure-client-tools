// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.Azure.Devices.Client;
using Microsoft.Azure.Devices.Shared;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;

namespace DMMockClient
{
    class DeviceTwinClient
    {
        public DeviceTwinClient(DeviceClient deviceClient)
        {
            _deviceClient = deviceClient;
        }

        // For some reason, calls to this function takes 1 minute.
        public async Task UpdateReportedPropertiesAsync(string name, object value, LogDelegateType Log)
        {
            Log("Updating '" + name + "'...");

            TwinCollection azureCollection = new TwinCollection();
            azureCollection[name] = value;

            try
            {
                await _deviceClient.UpdateReportedPropertiesAsync(azureCollection);
            }
            catch (Exception e)
            {
                Log("Error: failed to update reported properties. " + e.Message);
            }
        }

        public async Task UpdateReportedPropertiesAsync(JObject reportedObject, LogDelegateType Log)
        {
            Log("Updating reported properties...");

            TwinCollection azureCollection = new TwinCollection();

            foreach (JProperty p in reportedObject.Children())
            {
                azureCollection[p.Name] = p.Value;
            }

            try
            {
                await _deviceClient.UpdateReportedPropertiesAsync(azureCollection);
            }
            catch (Exception e)
            {
                Log("Error: failed to update reported properties. " + e.Message);
            }
        }

        public async Task<JObject> GetRootAsync()
        {
            var twin = await _deviceClient.GetTwinAsync();

            JObject desired = new JObject();

            foreach (KeyValuePair<string, object> p in twin.Properties.Desired)
            {
                desired[p.Key] = (JToken)p.Value;
            }

            JObject reported = new JObject();

            foreach (KeyValuePair<string, object> p in twin.Properties.Reported)
            {
                reported[p.Key] = (JToken)p.Value;
            }

            JObject properties = new JObject();
            properties["desired"] = desired;
            properties["reported"] = reported;

            JObject root = new JObject();
            root["properties"] = properties;

            return root;
        }

        private DeviceClient _deviceClient;
    }
}
