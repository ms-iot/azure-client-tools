:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.

@echo off

@goto START

:Usage
echo Usage: deploy.create-sdk-folder.cmd folder
echo    [/?].................... Displays this usage string.
echo    Example:
echo        deploy.create-sdk-folder.cmd folder
endlocal
exit /b 1

:START
setlocal ENABLEDELAYEDEXPANSION

if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage

if [%1] == [] ( 
    set ROOT_FOLDER=.
) else (
    set ROOT_FOLDER=%1
)

pushd %~dp0

@REM Utilities

set UtilitiesIncludes=%ROOT_FOLDER%\includes\utilities
echo UtilitiesIncludes=%UtilitiesIncludes%

md %UtilitiesIncludes%
call :CopyFile ..\code\utilities\*.h %UtilitiesIncludes%\

@REM Utilities\json

set UtilitiesJsonIncludes=%ROOT_FOLDER%\includes\utilities\Json
echo UtilitiesJsonIncludes=%UtilitiesJsonIncludes%

md %UtilitiesJsonIncludes%
call :CopyFile ..\code\utilities\json\*.h %UtilitiesJsonIncludes%\

@REM Common

set CommonIncludes=%ROOT_FOLDER%\includes\common
echo CommonIncludes=%CommonIncludes%

md %CommonIncludes%
call :CopyFile ..\code\device-agent\common\*.h %CommonIncludes%\

@REM Common\Plugins

set CommonPluginsIncludes=%CommonIncludes%\plugins
echo CommonPluginsIncludes=%CommonPluginsIncludes%

md %CommonPluginsIncludes%
call :CopyFile ..\code\device-agent\common\plugins\*.h %CommonPluginsIncludes%\

@REM Common\CSPs

set CommonCSPsIncludes=%CommonIncludes%\csps
echo CommonCSPsIncludes=%CommonCSPsIncludes%

md %CommonCSPsIncludes%
call :CopyFile ..\code\device-agent\common\csps\*.h %CommonCSPsIncludes%\

@REM PluginCommon

set PluginCommonIncludes=%ROOT_FOLDER%\includes\plugin-common
echo PluginCommonIncludes=%PluginCommonIncludes%

md %PluginCommonIncludes%
call :CopyFile ..\code\device-agent\plugin-common\*.h %PluginCommonIncludes%\


for %%Z in (x86 arm x64) do (

    for %%Y in (Debug Release) do (

    @REM for %%Y in (Debug) do (

        if [%%Z] == [x86] (
            set SOURCE_ARCH=Win32
        ) else (
            set SOURCE_ARCH=%%Z
        )

        if [%%Z] == [x64] (
            set TARGET_ARCH=amd64
        ) else (
            set TARGET_ARCH=%%Z
        )

        md %ROOT_FOLDER%\!TARGET_ARCH!\%%Y

        call :CopyFile ..\code\output\!SOURCE_ARCH!\%%Y\AzureDeviceManagementCommon.lib %ROOT_FOLDER%\!TARGET_ARCH!\%%Y\
        call :CopyFile ..\code\output\!SOURCE_ARCH!\%%Y\AzureDeviceManagementPluginCommon.lib %ROOT_FOLDER%\!TARGET_ARCH!\%%Y\
        call :CopyFile ..\code\output\!SOURCE_ARCH!\%%Y\AzureDeviceManagementUtilities.lib %ROOT_FOLDER%\!TARGET_ARCH!\%%Y\
        call :CopyFile ..\code\output\!SOURCE_ARCH!\%%Y\Platform.lib %ROOT_FOLDER%\!TARGET_ARCH!\%%Y\
        call :CopyFile ..\code\output\!SOURCE_ARCH!\%%Y\Urchin.lib %ROOT_FOLDER%\!TARGET_ARCH!\%%Y\

   )
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