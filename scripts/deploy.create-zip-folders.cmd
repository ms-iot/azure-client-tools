:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components and DPS app
@echo off

goto START

:Usage
echo Usage: deploy.create-zip-folders.cmd folder version
echo    [/?].................... Displays this usage string.
echo    Example:
echo        deploy.create-zip-folders.cmd c:\zipfolders 2.0.0.2
endlocal
exit /b 1

:START

pushd %~dp0

setlocal ENABLEDELAYEDEXPANSION

if [%1] == [] goto Usage
if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage
if [%2] == [] goto Usage

set ROOT_FOLDER=%1
set DEVICE_FOLDER=%ROOT_FOLDER%\Device
set ZIP_FILE_VERSION=%2
set ZIP_UTILITY=%~dp0..\code\Tools\ZipHelperUtility\bin\Debug\ZipHelperUtility.exe

if not exist %ZIP_UTILITY% (
    echo.
    echo Error: Missing %ZIP_UTILITY%
    echo.
    echo Make sure you build: code\Tools\ZipHelperUtility\ZipHelperUtility.sln
    exit /b 1
)

for %%Z in (x86 arm x64) do (
    echo Populating %%Z folder...
    call deploy.create-device-folder.cmd %%Z Debug %DEVICE_FOLDER%\%%Z
    echo placeholder > %DEVICE_FOLDER%\%%Z\logs\placeholder.txt

    %ZIP_UTILITY% %DEVICE_FOLDER%\%%Z %~dp0..\downloads\device.agent.%ZIP_FILE_VERSION%.%%Z.zip
)

popd