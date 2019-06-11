// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Graphics.Display;
using Windows.Media.Capture;
using Windows.System.Display;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CompanionQROOB
{
    class VideoPreview
    {
        static readonly Guid RotationGuid = new Guid("C380465D-2271-428C-9B83-ECEA3B4A85C1");

        public VideoPreview(CoreDispatcher dispatcher, CaptureElement captureElement)
        {
            _dispatcher = dispatcher;
            _captureElement = captureElement;
        }

        private async Task SetPreviewRotationAsync(string selectedScannerVideoDeviceId, DisplayOrientations displayOrientation)
        {
            bool isExternalCamera;
            bool isPreviewMirrored;

            // Figure out where the camera is located to account for mirroring and later adjust rotation accordingly.
            DeviceInformation cameraInformation = await DeviceInformation.CreateFromIdAsync(selectedScannerVideoDeviceId);

            if ((cameraInformation.EnclosureLocation == null) || (cameraInformation.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown))
            {
                isExternalCamera = true;
                isPreviewMirrored = false;
            }
            else
            {
                isExternalCamera = false;
                isPreviewMirrored = (cameraInformation.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Front);
            }

            _captureElement.FlowDirection = isPreviewMirrored ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;

            if (!isExternalCamera)
            {
                // Calculate which way and how far to rotate the preview.
                int rotationDegrees = 0;
                switch (displayOrientation)
                {
                    case DisplayOrientations.Portrait:
                        rotationDegrees = 90;
                        break;
                    case DisplayOrientations.LandscapeFlipped:
                        rotationDegrees = 180;
                        break;
                    case DisplayOrientations.PortraitFlipped:
                        rotationDegrees = 270;
                        break;
                    case DisplayOrientations.Landscape:
                    default:
                        rotationDegrees = 0;
                        break;
                }

                // The rotation direction needs to be inverted if the preview is being mirrored.
                if (isPreviewMirrored)
                {
                    rotationDegrees = (360 - rotationDegrees) % 360;
                }

                // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames.
                var streamProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview);
                streamProperties.Properties[RotationGuid] = rotationDegrees;
                await _mediaCapture.SetEncodingPropertiesAsync(MediaStreamType.VideoPreview, streamProperties, null);
            }
        }

        private async void ShowMessage(string message)
        {
            await _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                var messageDialog = new MessageDialog(message);
                messageDialog.ShowAsync();
            });
        }

        private void OnMediaCaptureFailed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            ShowMessage("Media capture failed. Make sure the camera is still connected.");
        }

        public async Task StartStreamingAsync(string videoDeviceId)
        {
            _mediaCapture = new MediaCapture();
            _mediaCapture.Failed += OnMediaCaptureFailed;

            var settings = new MediaCaptureInitializationSettings
            {
                VideoDeviceId = videoDeviceId,
                StreamingCaptureMode = StreamingCaptureMode.Video,
                SharingMode = MediaCaptureSharingMode.SharedReadOnly,
            };

            // Initialize MediaCapture
            bool captureInitialized = false;
            try
            {
                await _mediaCapture.InitializeAsync(settings);
                captureInitialized = true;
            }
            catch (UnauthorizedAccessException)
            {
                ShowMessage("The app was denied access to the camera.");
            }
            catch (Exception e)
            {
                ShowMessage("Failed to initialize the camera." + e.Message);
            }

            if (captureInitialized)
            {
                // Prevent the device from sleeping while the preview is running.
                _displayRequest.RequestActive();

                _captureElement.Source = _mediaCapture;
                await _mediaCapture.StartPreviewAsync();
                await SetPreviewRotationAsync(videoDeviceId, DisplayInformation.GetForCurrentView().CurrentOrientation);
            }
            else
            {
                _mediaCapture.Dispose();
                _mediaCapture = null;
            }
        }

        // Data members...
        private CoreDispatcher _dispatcher;
        private CaptureElement _captureElement;
        private MediaCapture _mediaCapture;
        private DisplayRequest _displayRequest = new DisplayRequest();
    }
}
