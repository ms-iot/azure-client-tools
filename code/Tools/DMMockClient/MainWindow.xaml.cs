// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Microsoft.Azure.Devices.Client;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.Windows;
using System.Windows.Controls;

namespace DMMockClient
{
    delegate void LogDelegateType(string msg);

    class DeviceSummary
    {
        public string DeviceId { get; set; }
        public string ConnectionString { get; set; }
    }

    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            LoadDeviceListData();
            RebuildDeviceList();
        }

        private void LoadDeviceListData()
        {
            _deviceSummaries = new List<DeviceSummary>();

            RegistryKey key = Registry.CurrentUser.OpenSubKey(DMConstants.RegistryStore);
            if (key == null)
            {
                return;
            }

            string[] valueNames = key.GetSubKeyNames();
            foreach (string valueName in valueNames)
            {
                RegistryKey subKey = Registry.CurrentUser.OpenSubKey(DMConstants.RegistryStore + "\\" + valueName);
                if (subKey == null)
                {
                    continue;
                }
                DeviceSummary ds = new DeviceSummary();
                ds.DeviceId = valueName;
                ds.ConnectionString = (string)subKey.GetValue(DMConstants.RegistryConnectionString);

                _deviceSummaries.Add(ds);
            }
        }

        private void SaveDeviceListData(string deviceId, string connectionString)
        {
            RegistryKey key = Registry.CurrentUser.CreateSubKey(DMConstants.RegistryStore + "\\" + deviceId);
            if (key == null)
            {
                return;
            }

            key.SetValue(DMConstants.RegistryConnectionString, connectionString);
        }

        private void DeleteDeviceListData(string deviceId)
        {
            RegistryKey key = Registry.CurrentUser.CreateSubKey(DMConstants.RegistryStore);
            if (key == null)
            {
                return;
            }

            key.DeleteSubKey(deviceId);
        }

        private void RebuildDeviceList()
        {
            DeviceList.ItemsSource = null;
            DeviceList.ItemsSource = _deviceSummaries;
        }

        private void OnSendTelemetry(object sender, RoutedEventArgs e)
        {
            foreach (DeviceSummary ds in DeviceList.SelectedItems)
            {
                DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(ds.ConnectionString, TransportType.Mqtt);
                TelemetryClient telemetry = new TelemetryClient(deviceClient);
                telemetry.SendTelemetry(TemperatureBox.Text, HumidityBox.Text, PressureBox.Text);
            }
        }

        private async void SendReportedAsync()
        {
            JObject reportedValues = (JObject)JsonConvert.DeserializeObject(ReportedPropertyValueBox.Text);

            uint count = 0;

            foreach (DeviceSummary ds in DeviceList.SelectedItems)
            {
                Log("Updating device '" + ds.DeviceId + "'...");

                DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(ds.ConnectionString, TransportType.Mqtt);
                DeviceTwinClient deviceTwinClient = new DeviceTwinClient(deviceClient);
                await deviceTwinClient.UpdateReportedPropertiesAsync(reportedValues, Log);

                ++count;
            }

            Log("Sent to " + count + " devices.");
            MessageBox.Show("Sent to " + count + " devices.");
        }

        private void OnSendReported(object sender, RoutedEventArgs e)
        {
            SendReportedAsync();
        }

        private async void GetPropertiesAsync()
        {
            if (DeviceList.SelectedItems.Count != 1)
            {
                MessageBox.Show("Only one device can be selected.");
                return;
            }

            DeviceSummary ds = (DeviceSummary)DeviceList.SelectedItem;

            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(ds.ConnectionString, TransportType.Mqtt);
            DeviceTwinClient deviceTwinClient = new DeviceTwinClient(deviceClient);

            JObject root = await deviceTwinClient.GetRootAsync();

            DesiredPropertyValueBox.Text = root["properties"]["desired"].ToString();
            ReportedPropertyValueBox.Text = root["properties"]["reported"].ToString();
        }

        private void OnBrowseReported(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
            {
                ReportedPropertyValueBox.Text = File.ReadAllText(openFileDialog.FileName);
            }
        }

        private void OnPasteReported(object sender, RoutedEventArgs e)
        {
            ReportedPropertyValueBox.Text = JsonTemplates.ReportedProperties;
        }

        private void OnSaveDeviceInfo(object sender, RoutedEventArgs e)
        {
            string deviceId = DeviceNameBox.Text.ToLower();

            if (deviceId.Length == 0 || DeviceConnectionStringBox.Text.Length == 0)
            {
                MessageBox.Show("Both the device name and the connection string must to be filled.");
                return;
            }

            foreach (DeviceSummary deviceSummary in _deviceSummaries)
            {
                if (deviceSummary.DeviceId.ToLower() == deviceId)
                {
                    MessageBox.Show("Duplicate device id. Choose a unique device id.");
                    return;
                }
            }

            SaveDeviceListData(DeviceNameBox.Text, DeviceConnectionStringBox.Text);

            DeviceSummary ds = new DeviceSummary();
            ds.DeviceId = DeviceNameBox.Text;
            ds.ConnectionString = DeviceConnectionStringBox.Text;

            _deviceSummaries.Add(ds);
            _deviceSummaries.Sort((x, y) => x.DeviceId.CompareTo(y.DeviceId));

            RebuildDeviceList();
            DeviceList.SelectedItem = ds;
        }

        private void OnSelectedDeviceChanged(object sender, SelectionChangedEventArgs e)
        {
            if (DeviceList.SelectedItems.Count == 0)
            {
                return;
            }

            if (DeviceList.SelectedItems.Count == 1)
            {
                DeviceSummary ds = (DeviceSummary)DeviceList.SelectedItem;

                DeviceNameBox.Text = ds.DeviceId;
                DeviceConnectionStringBox.Text = ds.ConnectionString;

                GetPropertiesAsync();
            }
            else
            {
                ClearDeviceDetails("<multiple>");
            }
        }

        private void ClearDeviceDetails(string msg)
        {
            DeviceNameBox.Text = msg;
            DeviceConnectionStringBox.Text = msg;
            DesiredPropertyValueBox.Text = msg;
            ReportedPropertyValueBox.Text = msg;
        }

        private void OnRemoveFromList(object sender, RoutedEventArgs e)
        {
            if (DeviceList.SelectedItems.Count == 0)
            {
                MessageBox.Show("An item must be selected to be removed.");
                return;
            }

            foreach (DeviceSummary ds in DeviceList.SelectedItems)
            {
                _deviceSummaries.Remove(ds);
                DeleteDeviceListData(ds.DeviceId);
            }

            RebuildDeviceList();

            ClearDeviceDetails("<empty>");
        }

        private void Log(string msg)
        {
            msg = DateTime.Now.ToString("hh:mm::ss") + ": " + msg;
            int index = LogListView.Items.Add(msg);
            LogListView.ScrollIntoView(LogListView.Items[index]);
        }

        List<DeviceSummary> _deviceSummaries;
    }
}
