@echo off
@REM Examples:
@REM   build.plugin-nuget.cmd 1.4.4
@REM   build.plugin-nuget.cmd 1.4.4

@if "%1"=="" goto MissingParameters

NuGet.exe pack "azure-device-agent-plugin-lib.nuspec" -Prop Version=%1
goto End

:MissingParameters
@echo.
@echo Usage:
@echo     build.plugin-nuget.cmd version
@echo.
@echo where:
@echo     version: the version of the nuget package to be genered in the form: major.minor.revision.
@echo.
@echo Example:
@echo     build.plugin-nuget.cmd 1.4.4
@echo.

:End
