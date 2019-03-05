:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components and DPS app
@echo off

goto START

:Usage
echo Usage: deploy.create-zip-folders.cmd folder
echo    [/?].................... Displays this usage string.
echo    Example:
echo        deploy.create-zip-folders.cmd c:\zipfolders
endlocal
exit /b 1

:START

pushd %~dp0

setlocal ENABLEDELAYEDEXPANSION

if [%1] == [] goto Usage
if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage

set ROOT_FOLDER=%1
set DEVICE_FOLDER=%ROOT_FOLDER%\Device

for %%Z in (x86 arm x64) do (
    echo Populating %%Z folder...
    call deploy.create-device-folder.cmd %%Z Debug %DEVICE_FOLDER%\%%Z

)

popd