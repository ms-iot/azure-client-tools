:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components and DPS app
@echo off

goto START

:Usage
echo Usage: build.azure-c-sdk.cmd x86^|ARM^|x64^|ARM64 Debug^|Release [WinSDKVer]
echo    WinSDKVer............... Default is 10.0.14393.0, specify another version if necessary
echo    [/?].................... Displays this usage string.
echo    Example:
echo        build.azure-c-sdk.cmd x64 Debug 10.0.16299.0
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

pushd %~dp0..\deps\azure-iot-sdk-c

echo .
echo "Using CMAKE to set up Azure projects"
echo.

md %TARGETARCH%
pushd %TARGETARCH%
if /I [%TARGETARCH%] == [x86] (
cmake -G "Visual Studio 15 2017" .. -Duse_prov_client:BOOL=ON -Duse_tpm_simulator:BOOL=OFF -Dbuild_provisioning_service_client=ON -Duse_prov_client_core=ON -Dskip_samples=ON -DCMAKE_SYSTEM_VERSION=%TARGETPLATVER%  ..
)

if /I [%TARGETARCH%] == [arm] (
cmake -G "Visual Studio 15 2017 ARM" .. -Duse_prov_client:BOOL=ON -Duse_tpm_simulator:BOOL=OFF -Dbuild_provisioning_service_client=ON -Duse_prov_client_core=ON -Dskip_samples=ON -DCMAKE_SYSTEM_VERSION=%TARGETPLATVER%  ..
)

if /I [%TARGETARCH%] == [x64] (
cmake -G "Visual Studio 15 2017 Win64" .. -Duse_prov_client:BOOL=ON -Duse_tpm_simulator:BOOL=OFF -Dbuild_provisioning_service_client=ON -Duse_prov_client_core=ON -Dskip_samples=ON -DCMAKE_SYSTEM_VERSION=%TARGETPLATVER%  ..
)

if /I [%TARGETARCH%] == [arm64] (
cmake -G "Visual Studio 15 2017" -A ARM64 .. -Duse_prov_client:BOOL=ON -Duse_tpm_simulator:BOOL=OFF -Dbuild_provisioning_service_client=ON -Duse_prov_client_core=ON -Dskip_samples=ON -DCMAKE_SYSTEM_VERSION=%TARGETPLATVER%  ..
)

popd

echo .
echo "Building Azure SDK libraries"
echo .

pushd %TARGETARCH%
msbuild c-utility\aziotsharedutil.vcxproj /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError
msbuild iothub_client\iothub_client.vcxproj  /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError
msbuild deps\uhttp\uhttp.vcxproj /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError
msbuild uamqp\uamqp.vcxproj /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError
msbuild provisioning_client\prov_device_ll_client.vcxproj /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError
msbuild provisioning_client\prov_http_transport.vcxproj  /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError
msbuild iothub_client\iothub_client_amqp_transport.vcxproj /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError
msbuild iothub_client\iothub_client_amqp_ws_transport.vcxproj /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError
msbuild iothub_service_client\iothub_service_client.vcxproj /p:Configuration=%TARGETCONFIG% /p:TargetPlatformVersion=%TARGETPLATVER%
if errorlevel 1 goto BuildError


popd

goto Success

:BuildError
popd
@echo Error building project...
exit /b 1

:Success

