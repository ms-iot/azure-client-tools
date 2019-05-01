// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.Azure.Devices;

namespace DMValidator
{
    class CloudServices
    {
        public ServiceClient IoTServiceClient { get; private set; }
        public IoTHubManager IoTHubManager { get; private set; }
        public RegistryManager IoTRegistryManager { get; private set; }
        public string AzureStorageConnectionString { get; private set; }

        public CloudServices(string iotHubConnectionString, string storageConnectionString)
        {
            if (IoTServiceClient == null)
            {
                IoTServiceClient = ServiceClient.CreateFromConnectionString(iotHubConnectionString);
            }

            if (IoTHubManager == null)
            {
                IoTHubManager = new IoTHubManager(iotHubConnectionString);
            }

            if (IoTRegistryManager == null)
            {
                IoTRegistryManager = RegistryManager.CreateFromConnectionString(iotHubConnectionString);
            }

            AzureStorageConnectionString = storageConnectionString;
        }
    }
}