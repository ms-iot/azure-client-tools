:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components and DPS app
@echo off

goto START

:Usage
echo Usage: build.cmd x86^|ARM^|x64 Debug^|Release [WinSDKVer]
echo    WinSDKVer............... Default is 10.0.14393.0, specify another version if necessary
echo    [/?].................... Displays this usage string.
echo    Example:
echo        build.cmd x64 Debug 10.0.16299.0
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
    set TARGETPLATVER=10.0.16299.0
) else (
    set TARGETPLATVER=%3
)

set LOGFILE=results.%TARGETARCH%.%TARGETCONFIG%.%TARGETPLATVER%.log

pushd %~dp0


call build.azure-c-sdk.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER%
if errorlevel 1 (
    echo build.azure-c-sdk.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER% >> %LOGFILE%
) else (
    echo OK build.azure-c-sdk.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER% >> %LOGFILE%
)

call build.azure-storage-cpp.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER%
if errorlevel 1 (
    echo build.azure-storage-cpp.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER% >> %LOGFILE%
) else (
    echo OK build.azure-storage-cpp.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER% >> %LOGFILE%
)

call build.azure-dm.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER%
if errorlevel 1 (
    echo build.azure-dm.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER% >> %LOGFILE%
) else (
    echo OK build.azure-dm.cmd %TARGETARCH% %TARGETCONFIG% %TARGETPLATVER% >> %LOGFILE%
)

@echo -----------------------------------------------------------------------
@echo -- Results ------------------------------------------------------------
@echo.
@type %LOGFILE%
@echo.