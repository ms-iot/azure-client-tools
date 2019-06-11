// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#include "..\..\..\..\deps\QR-Code-generator\cpp\BitBuffer.hpp"
#include "..\..\..\..\deps\QR-Code-generator\cpp\QrCode.hpp"

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

using namespace QROOB;
using namespace concurrency;

using namespace Platform;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

using namespace std;

using namespace DMBridgeComponent;

// https://docs.microsoft.com/en-us/windows/desktop/api/winsvc/nf-winsvc-changeserviceconfiga
enum ServiceStartType
{
    SERVICE_START_AUTO = 0x00000002,
    SERVICE_START_BOOT = 0x00000000,
    SERVICE_START_DEMAND = 0x00000003,
    SERVICE_DISABLED_ = 0x00000004,
    SERVICE_START_SYSTEM = 0x00000001
};

enum StartupType
{
    eUndefined = 0x00000000,
    eNone = 0x00000001,
    eForeground = 0x00000002,
    eBackground = 0x00000003
};

wstring MultibyteToWide(const char* s)
{
    size_t length = s ? strlen(s) : 0;
    size_t requiredCharCount = MultiByteToWideChar(CP_UTF8, 0, s, static_cast<int>(length), nullptr, 0);

    // add room for \0
    ++requiredCharCount;

    vector<wchar_t> wideString(requiredCharCount);
    MultiByteToWideChar(CP_UTF8, 0, s, static_cast<int>(length), wideString.data(), static_cast<int>(wideString.size()));

    return wstring(wideString.data());
}

string WideToMultibyte(const wchar_t* s)
{
    size_t length = s ? wcslen(s) : 0;
    size_t requiredCharCount = WideCharToMultiByte(CP_UTF8, 0, s, static_cast<int>(length), nullptr, 0, nullptr, nullptr);

    // add room for \0
    ++requiredCharCount;

    vector<char> multibyteString(requiredCharCount);
    WideCharToMultiByte(CP_UTF8, 0, s, static_cast<int>(length), multibyteString.data(), static_cast<int>(multibyteString.size()), nullptr, nullptr);

    return string(multibyteString.data());
}

MainPage::MainPage()
{
    InitializeComponent();

    TimeSpan ts;
    ts.Duration = 10 * 1000;

    _dispatcherTimer = ref new DispatcherTimer();
    _dispatcherTimer->Interval = ts;
    _dispatcherTimer->Start();
    _dispatcherTimer->Tick += ref new EventHandler<Object^>(this, &MainPage::OnReadTpmInfo);
}

void MainPage::OnReadTpmInfo(Object^ sender, Object^ e)
{
    _dispatcherTimer->Stop();
    ReadTpmInfo();
}

void MainPage::DpsEnrollmentInfo::Load()
{
    TpmBridge^ tpmBridge = ref new TpmBridge();

    try
    {
        registrationId = tpmBridge->GetRegistrationId();
        endorsementKey = tpmBridge->GetEndorsementKey();
    }
    catch (Exception^ ex)
    {
        registrationId = ex->Message;
        endorsementKey = ex->Message;
    }
}

std::string MainPage::DpsEnrollmentInfo::ToJsonString() const
{
    string s;

    s += "{";
    s += "\"regid\":\"";
    s += WideToMultibyte(registrationId->Data());
    s += "\",\"ek\":\"";
    s += WideToMultibyte(endorsementKey->Data());
    s += "\"";
    s += "}";

    return s;
}

std::string MainPage::DpsEnrollmentInfo::ToSvgString() const
{
    const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW;
    const QrCode qr = QrCode::encodeText(ToJsonString().c_str(), errCorLvl);
    return qr.toSvgString(4 /*border*/);
}

wstring MainPage::WriteLocalFile(
    const string& content,
    const wstring& fileName)
{
    auto filePath = wstring(ApplicationData::Current->LocalFolder->Path->Data()) + L"\\" + fileName;
    HANDLE fileHandle = CreateFile2(filePath.c_str(), GENERIC_WRITE, 0, CREATE_ALWAYS, nullptr);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        auto writeResult = WriteFile(fileHandle, content.data(), static_cast<DWORD>(content.length()), &bytesWritten, nullptr);
        if (writeResult == FALSE || bytesWritten != content.length())
        {
            throw runtime_error("Failed to write content message to the content file: error code " + to_string(GetLastError()));
        }
        CloseHandle(fileHandle);
    }

    return filePath;
}

void MainPage::ReadTpmInfo()
{
    // Load the data from the TPM...
    DpsEnrollmentInfo info;
    info.Load();

    // Load the data in the UI...
    RegIdBox->Text = info.registrationId;
    EKBox->Text = info.endorsementKey;

    // Save it to an image...
    wstring fullFileName = WriteLocalFile(info.ToSvgString(), L"qrd.svg");

    // Load the image in the UI...
    Uri^ uri = ref new Uri(ref new Platform::String(fullFileName.c_str()));
    SvgImageSource^ svgImageSource = ref new SvgImageSource(uri);
    QrImage->Source = svgImageSource;
}

void MainPage::OnDone(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StorageFolder^ appInstalledFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;

    auto task1 = create_task(appInstalledFolder->GetFileAsync(ref new Platform::String(L"finalize.config.json"))).then([](task<StorageFile^> fileTask)
    {
        StorageFile^ file = fileTask.get();
        if (file != nullptr)
        {
            auto task2 = create_task(FileIO::ReadTextAsync(file)).then([](task<String^> contentTask)
            {
                try
                {
                    String^ fileContent = contentTask.get();

                    JsonObject^ jsonObject = JsonObject::Parse(fileContent);
                    String^ startupService = jsonObject->Lookup("startupService")->GetString();
                    String^ startupApp = jsonObject->Lookup("startupApp")->GetString();

                    // Set the service to be auto-start
                    NTServiceBridge^ ntServiceBridge = ref new NTServiceBridge();
                    ntServiceBridge->SetStartMode(startupService, ServiceStartType::SERVICE_START_AUTO);

                    // Enable the OEM app to be the foreground app (replacing the OOB app)
                    UwpAppMgmtBridge^ uwpAppMgmt = ref new UwpAppMgmtBridge();
                    uwpAppMgmt->SetAppStartup(startupApp, StartupType::eForeground);

                    // Shutdown the device...
                    ShutdownMgmtBridge^ shutdownMgmt = ref new ShutdownMgmtBridge();
                    shutdownMgmt->Shutdown(10, true);
                }
                catch (COMException^ ex)
                {
                    if (ex->HResult == HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION))
                    {
                    }
                    else
                    {
                    }
                }
            });
        }
    });
}
