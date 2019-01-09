:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script builds CPPREST and AZURE-STORAGE-CPP for IoT (ARM, x86, & x64).
@echo off

goto START

:Usage
echo Usage: build.azure-storage-cpp.cmd x86^|ARM^|x64 Debug^|Release [WinSDKVer]
echo    WinSDKVer............... Default is 10.0.14393.0, specify another version if necessary
echo    [/?].................... Displays this usage string.
echo    Example:
echo        build.azure-storage-cpp.cmd x64 Debug 10.0.16299.0
endlocal
exit /b 1

:START
setlocal ENABLEDELAYEDEXPANSION

if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage

if [%1] == [] (
    set TARGETARCH=x64
) else (
    if [%1] == [x86] (
        set TARGETARCH=Win32
    ) else (
        set TARGETARCH=%1
    )
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

where /q nuget.exe
if not !errorlevel! == 0 (
choice /C yn /M "nuget.exe was not found.  Do you want to download nuget.exe from https://dist.nuget.org/win-x86-commandline/latest/nuget.exe?" 
if not !errorlevel!==1 goto :eof
rem if nuget.exe is not found, then ask user
Powershell.exe wget -outf nuget.exe https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
    if not exist .\nuget.exe (
        echo nuget does not exist
        goto BuildError
    )
)

set DEPS_DIR=%~dp0\..\deps
if /I [%TARGETCONFIG%] == [Debug] ( 
    set CASABLANCALIBRARY=cpprest141d_2_10.lib
) else (
    set CASABLANCALIBRARY=cpprest141_2_10.lib
)
set CASABLANCAINCLUDEPATH=%DEPS_DIR%\cpprestsdk\Release\include

echo .
echo nuget.exe restore %DEPS_DIR%\cpprestsdk\cpprestsdk141.sln
echo .
nuget.exe restore %DEPS_DIR%\cpprestsdk\cpprestsdk141.sln
if errorlevel 1 goto BuildError

echo .
echo nuget.exe restore %DEPS_DIR%\azure-storage-cpp\Microsoft.WindowsAzure.Storage.v141.sln
echo .
nuget.exe restore %DEPS_DIR%\azure-storage-cpp\Microsoft.WindowsAzure.Storage.v141.sln
if errorlevel 1 goto BuildError

echo .
echo msbuild /t:Build %DEPS_DIR%\cpprestsdk\Release\src\build\vs141\cpprest141.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER% /p:Configuration=%TARGETCONFIG% /p:Platform=%TARGETARCH%
echo .
msbuild /t:Build %DEPS_DIR%\cpprestsdk\Release\src\build\vs141\cpprest141.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER% /p:Configuration=%TARGETCONFIG% /p:Platform=%TARGETARCH%
if errorlevel 1 goto BuildError

echo .
echo msbuild /t:Build %DEPS_DIR%\azure-storage-cpp\Microsoft.WindowsAzure.Storage\Microsoft.WindowsAzure.Storage.v141.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER% /p:Configuration=%TARGETCONFIG% /p:Platform=%TARGETARCH% /p:CASABLANCALIBRARY=%CASABLANCALIBRARY% /p:CASABLANCAINCLUDEPATH="%CASABLANCAINCLUDEPATH%" /p:CASABLANCALIBRARYPATH="%DEPS_DIR%\cpprestsdk\Binaries\%TARGETARCH%\%TARGETCONFIG%"
echo .
msbuild /t:Build %DEPS_DIR%\azure-storage-cpp\Microsoft.WindowsAzure.Storage\Microsoft.WindowsAzure.Storage.v141.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER% /p:Configuration=%TARGETCONFIG% /p:Platform=%TARGETARCH% /p:CASABLANCALIBRARY=%CASABLANCALIBRARY% /p:CASABLANCAINCLUDEPATH="%CASABLANCAINCLUDEPATH%" /p:CASABLANCALIBRARYPATH="%DEPS_DIR%\cpprestsdk\Binaries\%TARGETARCH%\%TARGETCONFIG%"
if errorlevel 1 goto BuildError

goto Success

:BuildError
@echo Error building project...
exit /b 1

:Success