@echo off

if [%1]==[go] goto :go

echo.
echo This a very simple script that runs all the tests for AzureDeviceManagementClient.exe.
echo.
echo You need to edit the file and adjust the various parameters according to your configuration.
echo.
echo When ready, re-run this script with the "go" parameter from an admin console window; for example:
echo.
echo    test.all.cmd go
echo.
echo All improvements are welcome :)
echo.

goto :eof

:go

@REM Iot Hub
set IoTHubConnectionString=<iothub owner connection string>
set DeviceConnectionString=<device connection string>
set DeviceId=<device id>

@REM Client - this is the output of using deploy.create-device-folder.cmd
@REM
set ClientFolder=<deployment folder>
set ClientConfigFile=%ClientFolder%\AzureDeviceManagementClient.json
set ClientExecutableFile=%ClientFolder%\AzureDeviceManagementClient.exe

@REM Test - double slashes are necessary when used in a json file - do not remove)
@REM
set TestWorkingFolderEscaped=<test working folder with back slashes escaped>
set TestWorkingFolder=<test working folder>

set TestCasesRootFolderEscaped=..\\Common\\TestCases
set TestConfigFile=%TestWorkingFolder%\test.config.json

md %TestWorkingFolder%

@REM
echo Creating Test Config File: %TestConfigFile%
@REM

echo { > %TestConfigFile%
echo     "connection-string" : "%IoTHubConnectionString%", >> %TestConfigFile%
echo     "tests" : "%TestCasesRootFolderEscaped%", >> %TestConfigFile%
echo     "devices" : [ >> %TestConfigFile%
echo         "%DeviceId%" >> %TestConfigFile%
echo     ], >> %TestConfigFile%
echo     "log" : "%TestWorkingFolderEscaped%" >> %TestConfigFile%
echo } >> %TestConfigFile%

@REM
echo Creating Client Config File: %ClientConfigFile%
@REM

echo { > %ClientConfigFile%
echo   "textLogFilesPath": "..\\logs", >> %ClientConfigFile%
echo   "pluginManifestsPath": "..\\manifests", >> %ClientConfigFile%
echo   "sasExpiryInterval": 86400, >> %ClientConfigFile%
echo   "sasRenewalGracePeriod": 3600, >> %ClientConfigFile%
echo   "dpsUri": "global.azure-devices-provisioning.net", >> %ClientConfigFile%
echo   "dpsScopeId": "", >> %ClientConfigFile%
echo   "connectionStringSlotNumber": 0, >> %ClientConfigFile%
echo   "debugConnectionString": "%DeviceConnectionString%", >> %ClientConfigFile%
echo   "moduleId": "dmModule", >> %ClientConfigFile%
echo   "handlers": { >> %ClientConfigFile%
echo     "deviceInfo": { >> %ClientConfigFile%
echo       "reportOnStart": true >> %ClientConfigFile%
echo     }, >> %ClientConfigFile%
echo     "__deviceSchemas": { >> %ClientConfigFile%
echo       "reportOnStart": true >> %ClientConfigFile%
echo     } >> %ClientConfigFile%
echo   } >> %ClientConfigFile%
echo } >> %ClientConfigFile%

echo Starting client...
@REM start cmd /c %ClientExecutableFile% -debug
powershell -Command "Start-Process %ClientExecutableFile% -ArgumentList '-debug' -Verb RunAs"

echo sleeping for 20 seconds...
timeout /t 20

pushd %~dp0..\code\Tools\DMValidator\CLI

dotnet bin\Debug\netcoreapp2.1\DMConsoleValidator.dll -C %TestConfigFile%

popd

:eof