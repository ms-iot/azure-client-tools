:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.

@echo off

goto START

:Usage
echo Usage: build.wdp.layout.cmd <folder>
echo    WinSDKVer............... Default is 10.0.14393.0, specify another version if necessary
echo    [/?].................... Displays this usage string.
echo    Example:
echo        build.wdp.layout.cmd c:\wdp.layout
endlocal
exit /b 1

:START

setlocal ENABLEDELAYEDEXPANSION

if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage

if [%1] == [] (
    set TARGETFOLDER=c:\wdp.layout
) else (
    set TARGETFOLDER=%1
)

pushd %~dp0

for %%Z in (x86 arm x64) do (

    echo Processing %%Z

    if [%%Z]==[x86] (
        set SOURCE_ARCH=Win32
    ) else (
        set SOURCE_ARCH=%%Z
    )

    md %TARGETFOLDER%\%%Z

    for /f %%Y in (%~dp0wdp\binaryfilelist.txt) do (
        copy ..\code\output\!SOURCE_ARCH!\Debug\%%Y %TARGETFOLDER%\%%Z\
    )

    for /f %%Y in (%~dp0wdp\manifestfilelist.txt) do (
        copy ..\code\device-agent-plugins\%%Y %TARGETFOLDER%\%%Z\
    )

    copy "c:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Redist\MSVC\14.16.27012\onecore\debug_nonredist\%%Z\Microsoft.VC141.DebugCRT\concrt140d.dll" %TARGETFOLDER%\%%Z\
    copy wdp\AzureDeviceManagementClient.json %TARGETFOLDER%\%%Z\
)

copy wdp\AzureClients.json %TARGETFOLDER%
copy wdp\TpmTools.json %TARGETFOLDER%
