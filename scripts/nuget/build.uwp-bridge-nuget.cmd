:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
@echo off

goto START

:Usage
@echo.
@echo Usage:
@echo     build.uwp-bridge-nuget.cmd version flavor
@echo.
@echo where:
@echo     version: the version of the nuget package to be genered in the form: major.minor.revision.
@echo     flavor : Debug or Release
@echo.
@echo Example:
@echo     build.uwp-bridge-nuget.cmd 2.0.0 Release
@echo.
endlocal
exit /b 1

:START
setlocal ENABLEDELAYEDEXPANSION

if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage
if [%1] == [] goto Usage

NuGet.exe pack "azure-device-agent-uwp-bridge-lib.nuspec" -Prop Version=%1 -Prop Flavor=%2
