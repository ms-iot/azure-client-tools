:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
@echo off

goto START

:Usage
@echo.
@echo Usage:
@echo     build.plugin-nuget.cmd version
@echo.
@echo where:
@echo     version: the version of the nuget package to be genered in the form: major.minor.revision.
@echo.
@echo Example:
@echo     build.plugin-nuget.cmd 2.0.0
@echo.
endlocal
exit /b 1

:START
setlocal ENABLEDELAYEDEXPANSION

if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage
if [%1] == [] goto Usage

NuGet.exe pack "azure-device-agent-plugin-lib.nuspec" -Prop Version=%1
