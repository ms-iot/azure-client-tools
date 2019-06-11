// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.Azure.Devices.Provisioning.Service;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using Windows.Devices.Enumeration;
using Windows.Devices.PointOfService;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CompanionQROOB
{
    public sealed partial class MainPage : Page
    {
        static readonly string JsonRegId = "regid";
        static readonly string JsonEK = "ek";

        public MainPage()
        {
            this.InitializeComponent();

            _videoPreview = new VideoPreview(Dispatcher, VideoPreviewControl);
        }

        private async void ShowMessage(string message)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                var messageDialog = new MessageDialog(message);
                messageDialog.ShowAsync();
            });
        }

        private async void OnInitializeScannerAsync()
        {
            string selector = BarcodeScanner.GetDeviceSelector(PosConnectionTypes.Local);
            DeviceInformationCollection deviceCollection = await DeviceInformation.FindAllAsync(selector);
            foreach (var device in deviceCollection)
            {
                BarcodeScanner scanner = await BarcodeScanner.FromIdAsync(device.Id);
                if (scanner != null && scanner.VideoDeviceId != null)
                {
                    _claimedScanner = await scanner.ClaimScannerAsync();
                    if (_claimedScanner != null)
                    {
                        await _claimedScanner.EnableAsync();

                        _claimedScanner.DataReceived += OnBarcodeDetected;

                        await _videoPreview.StartStreamingAsync(scanner.VideoDeviceId);

                        await _claimedScanner.StartSoftwareTriggerAsync();

                        break;
                    }
                }
            }
        }

        private void OnInitializeScanner(object sender, RoutedEventArgs e)
        {
            OnInitializeScannerAsync();
        }

        private async void OnBarcodeDetected(ClaimedBarcodeScanner sender, BarcodeScannerDataReceivedEventArgs args)
        {
            string regId = "<undefined>";
            string ek = "<undefined>";

            // Parse the data
            string decodedText = BarcodeDecoder.DecodeLabel(args.Report.ScanDataLabel);
            JObject jsonData = (JObject)JsonConvert.DeserializeObject(decodedText);
            if (jsonData.ContainsKey(JsonRegId))
            {
                regId = (string)jsonData[JsonRegId];
            }

            if (jsonData.ContainsKey(JsonEK))
            {
                ek = (string)jsonData[JsonEK];
            }

            // Update the UI
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                RegIdBox.Text = regId;
                EKBox.Text = ek;
            });
        }

        private async void EnrollAsync()
        {
            using (ProvisioningServiceClient provisioningServiceClient = ProvisioningServiceClient.CreateFromConnectionString(DPSConnectionStringBox.Text))
            {
                IndividualEnrollment individualEnrollment = new IndividualEnrollment(RegIdBox.Text, new TpmAttestation(EKBox.Text));

                // The following parameters are optional. Remove them if you don't need them.
                if (DeviceNameBox.Text.Length != 0)
                {
                    individualEnrollment.DeviceId = DeviceNameBox.Text;
                }
                individualEnrollment.ProvisioningStatus = ProvisioningStatus.Enabled;

                IndividualEnrollment individualEnrollmentResult =
                    await provisioningServiceClient.CreateOrUpdateIndividualEnrollmentAsync(individualEnrollment).ConfigureAwait(false);

                ++_deviceNameCounter;
                UpdateDeviceName();

                ShowMessage("Enrollment created successfully.");
            }
        }

        private void OnEnroll(object sender, RoutedEventArgs e)
        {
            EnrollAsync();
        }

        private async void UpdateDeviceName()
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                DeviceNameBox.Text = DeviceNameBaseBox.Text + _deviceNameCounter.ToString("D4");
            });
        }

        private void OnDeviceNameBaseChanged(object sender, TextChangedEventArgs e)
        {
            UpdateDeviceName();
        }

        private void OnDeviceCounterChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                _deviceNameCounter = UInt32.Parse(DeviceCounterBox.Text);
                UpdateDeviceName();
            }
            catch (Exception ex)
            {
                ShowMessage(ex.Message);
            }
        }

        // Data members...
        private ClaimedBarcodeScanner _claimedScanner;
        private VideoPreview _videoPreview;
        private uint _deviceNameCounter = 0;
    }
}
