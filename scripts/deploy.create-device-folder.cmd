:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components and DPS app
@echo off

goto START

:Usage
echo Usage: deploy.create-device-folder.cmd x86^|ARM^|x64 Debug^|Release folder
echo    [/?].................... Displays this usage string.
echo    Example:
echo        deploy.create-device-folder.cmd x64 Debug c:\deploy
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

pushd %~dp0

set ROOT_FOLDER=%TARGETFOLDER%
set EXECUTABLES_FOLDER=%ROOT_FOLDER%\bin
set PLUGIN_MANIFESTS_FOLDER=%ROOT_FOLDER%\manifests
set LOGS_FOLDER=%ROOT_FOLDER%\logs

if not exist %ROOT_FOLDER% mkdir %ROOT_FOLDER% > nul
if not exist %EXECUTABLES_FOLDER% mkdir %EXECUTABLES_FOLDER% > nul
if not exist %PLUGIN_MANIFESTS_FOLDER% mkdir %PLUGIN_MANIFESTS_FOLDER% > nul
if not exist %LOGS_FOLDER% mkdir %LOGS_FOLDER% > nul

if /I [%TARGETARCH%] == [x86] (
    set TARGETARCH_FOLDER=Win32\
    set LIMPET_TARGETARCH_FOLDER=
) else (
    set TARGETARCH_FOLDER=%TARGETARCH%\
    set LIMPET_TARGETARCH_FOLDER=%TARGETARCH%\
)

if /I [%TARGETCONFIG%] == [Debug] (
      set CASABLANCALIBRARY=cpprest141d_2_10
      set VCBINPATH="%VCToolsRedistDir%onecore\debug_nonredist\%TARGETARCH%\Microsoft.VC141.DebugCRT"
      set UCRTBINPATH="%WindowsSdkVerBinPath%%TARGETARCH%\ucrt"
) else (
      set CASABLANCALIBRARY=cpprest141_2_10
      set VCBINPATH="%VCToolsRedistDir%onecore\%TARGETARCH%\Microsoft.VC141.CRT"
)

echo.
echo Creating deploy folder for device-agent for [32m%TARGETARCH%[0m architecture and [32m%TARGETCONFIG%[0m config
echo.

@REM Executables
call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\Limpet.exe %EXECUTABLES_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\AzureDeviceManagementClient.exe %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent\agent\AzureDeviceManagementClient.json %EXECUTABLES_FOLDER%
call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\AzureDeviceManagementPluginHost.exe %EXECUTABLES_FOLDER%

@REM Storage Infrastructre
call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\%CASABLANCALIBRARY%.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\wastorage.dll %EXECUTABLES_FOLDER%

@REM Plugins

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\DiagnosticLogsManagementPlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\DiagnosticLogsManagementPlugin\DiagnosticLogsManagementManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\CertificateManagementPlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\CertificateManagementPlugin\CertificateManagementManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\DeviceInfoPlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\DeviceInfoPlugin\DeviceInfoManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\FactoryResetPlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\FactoryResetPlugin\FactoryResetManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\RebootPlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\RebootPlugin\RebootManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\RemoteWipePlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\RemoteWipePlugin\RemoteWipeManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\TimePlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\TimePlugin\TimeManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\WindowsTelemetryPlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\WindowsTelemetryPlugin\WindowsTelemetryManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\UwpAppManagementPlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\UwpAppManagementPlugin\UwpAppManagementManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\WindowsUpdatePlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\device-agent-plugins\WindowsUpdatePlugin\WindowsUpdateManifest.json %PLUGIN_MANIFESTS_FOLDER%

if /I [%TARGETCONFIG%] == [Debug] (
    for %%Y in (vccorlib140d.dll msvcp140d.dll vcruntime140d.dll concrt140d.dll) do (
        call :CopyFile %VCBINPATH%\%%Y %EXECUTABLES_FOLDER%
    )
    call :CopyFile %UCRTBINPATH%\ucrtbased.dll %EXECUTABLES_FOLDER%
) else (
    for %%Y in (vccorlib140.dll msvcp140.dll vcruntime140.dll concrt140.dll) do (
        call :CopyFile %VCBINPATH%\%%Y %EXECUTABLES_FOLDER%
    )
)

@REM -- Samples --
@echo.
@echo Copying Samples...
@echo.

@REM -- DirectRebootManagement causes a conflict with the build-in Reboot Management handler.
@REM call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\DirectRebootManagementPlugin.dll %EXECUTABLES_FOLDER%
@REM call :CopyFile ..\code\Samples\Plugins\DirectRebootManagementPlugin\DirectRebootManagementManifest.json %PLUGIN_MANIFESTS_FOLDER%

call :CopyFile ..\code\output\%TARGETARCH_FOLDER%%TARGETCONFIG%\TestPlugin.dll %EXECUTABLES_FOLDER%
call :CopyFile ..\code\Samples\Plugins\TestPlugin\TestManifest.json %PLUGIN_MANIFESTS_FOLDER%

if !errors! neq 0 (
    echo.
    echo XX On or more files failed to be copied.
    exit /b 1
) else (
    echo.
    echo Ok All files have been copied successfully.
    exit /b 0
)


popd

goto endoffile

:CopyFile
copy %1 %2 > nul
if %errorlevel% equ 0 (
    echo [OK] %1
) else (
    echo [31m[FAILED]%1[0m
    set errors=1
)
exit /B

:endoffile