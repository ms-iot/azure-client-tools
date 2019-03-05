:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components and DPS app
@echo off

goto START

:Usage
echo Usage: deploy.create-device-folder.cmd x86^|ARM^|x64 Debug^|Release folder [vs_bins_folder] [sdk_bins_folder]
echo    [/?].................... Displays this usage string.
echo    Example:
echo        deploy.create-device-folder.cmd x64 Debug c:\deploy "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Redist\MSVC\14.16.27012\onecore\debug_nonredist" "C:\Program Files (x86)\Microsoft SDKs\Windows Kits\10\ExtensionSDKs\Microsoft.UniversalCRT.Debug\10.0.16299.0\Redist\Debug"
endlocal
exit /b 1

:START
setlocal ENABLEDELAYEDEXPANSION

if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage

if [%1] == [] (
    set TARGETARCH=x64
) else (
    set TARGETARCH=%1
)

if [%2] == [] (
    set TARGETCONFIG=Debug
) else (
    set TARGETCONFIG=%2
)

if [%3] == [] ( 
    set TARGETFOLDER=.
) else (
    set TARGETFOLDER=%3
)

if [%4] == [] ( 
    set VS_DEBUG_BINS=C:\Program Files ^(x86^)\Microsoft Visual Studio\2017\Enterprise\VC\Redist\MSVC\14.16.27012\onecore\debug_nonredist
) else (
    set VS_DEBUG_BINS=%4
)

if [%5] == [] ( 
    set SDK_DEBUG_BINS=C:\Program Files ^(x86^)\Microsoft SDKs\Windows Kits\10\ExtensionSDKs\Microsoft.UniversalCRT.Debug\10.0.16299.0\Redist\Debug
) else (
    set SDK_DEBUG_BINS=%5
)

pushd %~dp0

set ROOT_FOLDER=%TARGETFOLDER%
set EXECUTABLES_FOLDER=%ROOT_FOLDER%\bin
set PLUGIN_MANIFESTS_FOLDER=%ROOT_FOLDER%\manifests
set LOGS_FOLDER=%ROOT_FOLDER%\logs

mkdir %ROOT_FOLDER%
mkdir %EXECUTABLES_FOLDER%
mkdir %PLUGIN_MANIFESTS_FOLDER%
mkdir %LOGS_FOLDER%

if /I [%TARGETARCH%] == [x86] (
    set TARGETARCH_FOLDER=Win32\
    set LIMPET_TARGETARCH_FOLDER=
) else (
    set TARGETARCH_FOLDER=%TARGETARCH%\
    set LIMPET_TARGETARCH_FOLDER=%TARGETARCH%\
)

if /I [%TARGETCONFIG%] == [Debug] (
      set CASABLANCALIBRARY=cpprest141d_2_10
) else (
      set CASABLANCALIBRARY=cpprest141_2_10
)

@REM Executables
copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\Limpet.exe %EXECUTABLES_FOLDER%
copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\AzureDeviceManagementClient.exe %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementClient\AzureDeviceManagementClient.json %EXECUTABLES_FOLDER%

copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\AzureDeviceManagementPluginHost.exe %EXECUTABLES_FOLDER%

@REM Storage Infrastructre
copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\%CASABLANCALIBRARY%.dll %EXECUTABLES_FOLDER%
copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\wastorage.dll %EXECUTABLES_FOLDER%

@REM Plugins
copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\CertificateManagementPlugin.dll %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementPlugins\CertificateManagementPlugin\CertificateManagementManifest.json %PLUGIN_MANIFESTS_FOLDER%

copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\DeviceInfoPlugin.dll %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementPlugins\DeviceInfoPlugin\DeviceInfoManifest.json %PLUGIN_MANIFESTS_FOLDER%

copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\FactoryResetPlugin.dll %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementPlugins\FactoryResetPlugin\FactoryResetManifest.json %PLUGIN_MANIFESTS_FOLDER%

copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\RebootPlugin.dll %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementPlugins\RebootPlugin\RebootManifest.json %PLUGIN_MANIFESTS_FOLDER%

copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\RemoteWipePlugin.dll %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementPlugins\RemoteWipePlugin\RemoteWipeManifest.json %PLUGIN_MANIFESTS_FOLDER%

copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\TimePlugin.dll %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementPlugins\TimePlugin\TimeManifest.json %PLUGIN_MANIFESTS_FOLDER%

copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\WindowsTelemetryPlugin.dll %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementPlugins\WindowsTelemetryPlugin\WindowsTelemetryManifest.json %PLUGIN_MANIFESTS_FOLDER%

copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\WindowsUpdatePlugin.dll %EXECUTABLES_FOLDER%
copy ..\code\AzureDeviceManagementPlugins\WindowsUpdatePlugin\WindowsUpdateManifest.json %PLUGIN_MANIFESTS_FOLDER%

for %%Y in (vccorlib140d.dll msvcp140d.dll vcruntime140d.dll concrt140d.dll) do (
    copy "%VS_DEBUG_BINS%\%TARGETARCH%\Microsoft.VC141.DebugCRT\%%Y" %EXECUTABLES_FOLDER%
)
copy "%SDK_DEBUG_BINS%\%TARGETARCH%\ucrtbased.dll" %EXECUTABLES_FOLDER%

@REM -- Samples --
@REM @echo.
@REM @echo Copying Samples...
@REM @echo.
@REM -- DirectRebootManagement causes a conflict with the build-in Reboot Management handler.
@REM copy ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\DirectRebootManagementPlugin.dll %EXECUTABLES_FOLDER%
@REM copy ..\code\Samples\Plugins\DirectRebootManagementPlugin\DirectRebootManagementManifest.json %PLUGIN_MANIFESTS_FOLDER%

popd