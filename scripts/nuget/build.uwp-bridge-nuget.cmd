@echo off
@REM Examples:
@REM   build.uwp-bridge-nuget.cmd 1.4.4 Release
@REM   build.uwp-bridge-nuget.cmd 1.4.4 Debug

@if "%1"=="" goto MissingParameters
@if "%2"=="" goto MissingParameters

NuGet.exe pack "azure-device-agent-uwp-bridge-lib.nuspec" -Prop Version=%1 -Prop Flavor=%2
goto End

:MissingParameters
@echo.
@echo Usage:
@echo     build.uwp-bridge-nuget.cmd version flavor
@echo.
@echo where:
@echo     version: the version of the nuget package to be genered in the form: major.minor.revision.
@echo     flavor : Debug or Release
@echo.
@echo Example:
@echo     build.uwp-bridge-nuget.cmd 1.4.4 Release
@echo.

:End
