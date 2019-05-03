:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components and DPS app
@echo off

goto START

:Usage
echo Usage: deploy.create-vs-template.cmd
echo    [/?].................... Displays this usage string.
echo    Example:
echo        deploy.create-vs-template.cmd
endlocal
exit /b 1

:START

pushd %~dp0

setlocal ENABLEDELAYEDEXPANSION

if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage

%~dp0..\code\Tools\ZipHelperUtility\bin\Debug\ZipHelperUtility.exe %~dp0..\code\device-agent-plugins\VSTemplatePlugin %~dp0..\downloads\AzureDeviceAgentPluginTemplate.zip

popd