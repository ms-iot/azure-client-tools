// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"

using namespace UwpAppUsingDMBridge;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace DMBridgeComponent;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
    InitializeComponent();
}

void UwpAppUsingDMBridge::MainPage::RebootButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto reboot = ref new RebootBridge();
    reboot->Invoke();
}

void UwpAppUsingDMBridge::MainPage::SetScopeIdButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto dps = ref new DpsBridge();
    dps->SetScopeId(this->ScopeId->Text);
}

void UwpAppUsingDMBridge::MainPage::GetRegIdButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto tpm = ref new TpmBridge();
    auto registrationId = tpm->GetRegistrationId();
    this->RegistrationIdText->Text = registrationId;
}

void UwpAppUsingDMBridge::MainPage::GetCxnStrButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto tpm = ref new TpmBridge();
    auto slotNumberString = this->SlotNumber->Text;
    auto slotNumberValue = _wtoi(slotNumberString->Data());
    auto cxnString = tpm->GetConnectionString(slotNumberValue, 36000);
    this->CxnStringText->Text = cxnString;
}

void UwpAppUsingDMBridge::MainPage::GetEkButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto tpm = ref new TpmBridge();
    auto ek = tpm->GetEndorsementKey();
    this->EkText->Text = ek;
}
