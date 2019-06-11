// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "MainPage.g.h"

namespace QROOB
{
    public ref class MainPage sealed
    {
        struct DpsEnrollmentInfo
        {
            Platform::String^ registrationId;
            Platform::String^ endorsementKey;

            void Load();

            std::string ToJsonString() const;
            std::string ToSvgString() const;
        };

    public:
        MainPage();

    private:
        void OnReadTpmInfo(
            Platform::Object^ sender,
            Platform::Object^ e);

        void OnDone(
            Platform::Object^ sender,
            Windows::UI::Xaml::RoutedEventArgs^ e);

        void ReadTpmInfo();

        std::wstring WriteLocalFile(
            const std::string& content,
            const std::wstring& fileName);

        static DpsEnrollmentInfo GetDpsEnrollmentInfo();

        Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile^>^ _sampleFile;
        Windows::UI::Xaml::DispatcherTimer^ _dispatcherTimer;
    };
}
