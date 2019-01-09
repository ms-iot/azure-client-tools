# Developer Machine Setup

## Install Visual Studio

- Visual Studio 2017 ([download](https://www.visualstudio.com/downloads)).
  - Make sure the following is selected:
    - VC++ 2017 v141 toolset (x86, x64)
    - Visual C++ compilers and libraries for ARM.
    - Visual C++ 2017 redistributable update.
    - Visual Studio C++ Core Features
    - Visual C++ ATL support
    - Windows 10 SDK (10.0.16299.0) for Desktop

## Install CMake

  - Install [v3.11](https://cmake.org/files/v3.11/cmake-3.11.0-win64-x64.msi) or the [latest](https://cmake.org/download/).
  - Add CMake to the PATH variable 

## Install DeviceExplorer

- Clone the Azure CSharp SDK

<pre>
git clone --recursive --branch master https://github.com/Azure/azure-iot-sdk-csharp.git azure-iot-sdk-csharp
</pre>

- Open `tools\DeviceExplorer\DeviceExplorer.sln` in Visual Studio, and build and run it.

## Cloning the Azure DM Repo

  - Clone the master branch of the device management repo recursively:

<pre>
cd /d d:\git
git clone --recursive --branch master https://github.com/ms-iot/windows-iot-azure-dm-standalone-client.git dm
</pre>

----

[Development Scenario](../development-scenario.md)