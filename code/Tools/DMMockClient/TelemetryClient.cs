// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.Azure.Devices.Client;
using System;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;

namespace DMMockClient
{
    class TelemetryClient
    {
        private const string DATE_FORMAT = "yyyy-MM-dd'T'HH:mm:sszzz";
        private const string CREATION_TIME_PROPERTY = "$$CreationTimeUtc";
        private const string MESSAGE_SCHEMA_PROPERTY = "$$MessageSchema";
        private const string CONTENT_PROPERTY = "$$ContentType";

        public TelemetryClient(DeviceClient deviceClient)
        {
            _deviceClient = deviceClient;
            _counter = 0;
        }

        //
        // E:\Projs\pcs.0312\azure-iot-pcs-remote-monitoring-dotnet\device-simulation\Services\DeviceClient.cs
        //

        private async Task SendRawMessageAsync(Message message)
        {
            try
            {
                await this._deviceClient.SendEventAsync(message);
                Debug.WriteLine("[" + DateTime.Now.ToString(DATE_FORMAT) + "] Sent a telemetry message...");

            }
            catch (Exception e)
            {
                Debug.WriteLine("[" + DateTime.Now.ToString(DATE_FORMAT) + "] " + e.Message);
            }
        }

        private async void SendMessageAsync(string message, string schemaName)
        {
            var eventMessage = new Message(Encoding.UTF8.GetBytes(message));
            eventMessage.Properties.Add(CREATION_TIME_PROPERTY, DateTimeOffset.UtcNow.ToString(DATE_FORMAT));
            eventMessage.Properties.Add(MESSAGE_SCHEMA_PROPERTY, schemaName);
            eventMessage.Properties.Add(CONTENT_PROPERTY, "JSON");

            await this.SendRawMessageAsync(eventMessage);
        }

        public void SendTelemetry(string temperature, string humidity, string pressure)
        {
            //
            // device-simulation\SimulationAgent\Simulation\DeviceStatusLogic\SendTelemetry.cs @ RunInternalAsync
            // device-simulation\Services\data\devicemodels\chiller-01.json
            //
            string temperature_unit = "F";
            string humidity_unit = "a";
            string pressure_unit = "psi";

            string schemaName = "chiller-sensors;v1";

            StringBuilder sb = new StringBuilder();
            sb.Append("{\"temperature\":" + temperature);
            sb.Append(",\"temperature_unit\":\"" + temperature_unit);
            sb.Append("\",\"humidity\":" + humidity);
            sb.Append(",\"humidity_unit\":\"" + humidity_unit);
            sb.Append("\",\"pressure\":" + pressure);
            sb.Append(",\"pressure_unit\":\"" + pressure_unit + "\"}");

            SendMessageAsync(sb.ToString(), schemaName);

            ++_counter;
        }

        private DeviceClient _deviceClient;
        private uint _counter;
    }
}
