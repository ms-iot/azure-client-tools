# Limpet.exe

####    Overview
 Limpet.exe allows local processess to use TPM for storing Azure connection strings, retrieve SAS tokens, register the device with IoT Hub using Azure DPS service and more.

#### TPM Support

| Board | Version | Meets Azure TPM Requirements |
|-------|---------|---------|
| DragonBoard | 2.0 | Yes |
| RP2/3 | - | No |
| MBM   | 2.0 | No |
| HummingBoard | ? | ? |


####    Command-line syntax
<pre>
Limpet.exe [Command]

 -VER => Show Limpet.exe version
 -LAT => List supported add-on TPMs
 -IAT [IDx] => Install specified add-on TPM

Global TPM Present Commands:
 -GTI => Get TPM Info
 -FCT => Force Clear TPM
 -PFX [PFXFile] [password] => Import PFX file into TPMKSP and machine "My" Store
 -LLD => List logical devices in use
 -ERK {File} => Get TPM Endorsement RSA Key and optionally store

Logical Device Commands in the form:
Limpet.exe [LogicalDeviceNo 0..9] [Command] [Parameter] {Optional}
 -RID => Read Device Id
 -SUR [URI] => Store Service URI
 -RUR => Read Service URI
 -DUR => Destroy Service URI

HMAC Symmetric Identities:
 -CHK [HmacKey] => Create persisted HMAC Key
 -AST {Validity} => Generate the SAS token-based connection string (default validity 3600s)
 -SHK [File] => Sign data in file with persisted HMAC Key
 -EHK => Evict persisted HMAC Key

Azure device provisioning cloud service(dps) commands in the form:
Limpet.exe [-azuredps [<dps connection string>] [command] [parameters]

 -azuredps -register <tpm slot number> <global dps uri end point> <scope id> => register the current running device in Iothub using dps service.
     <tpm slot number>: tpm slot number to store the key.
     <global dps service uri>: it is generally fixed global.azure-devices-provisioning.net
     <scope id>: customer dps scope id. you can get this information from azure portal dps service ->overview ->id scope.
 -azuredps -enrollmentinfo <csv/json/txt> => read device tpm information needed for enrolling the device later in dps and output in given format.

</pre>

#### Get from github
Clone recursively:

    git clone --recursive https://github.com/ms-iot/azure-dm-client

If you find that the deps folder is empty, try this:

    git submodule update --init --recursive

#### Set up development environment
Be sure you have CMAKE configured:

* Install [CMake](https://cmake.org/download/). 
* Make sure it is in your PATH by typing cmake -version from a command prompt. CMake will be used to create Visual Studio projects to build libraries and samples. 
* Make sure your CMake version is at least 3.6.1.

Be sure you have PERL configured:

* Install [perl](https://www.perl.org/get.html). You can use either ActivePerl or Strawberry Pearl. Run the installer as an administrator to avoid issues.
    
Be sure you are using Visual Studio 2017 with Visual C++ (this last bit is important!)

#### Build binaries for x86, ARM and X64

    Start a VS 2017 developer command prompt
    cd <repo>
    build.all.cmd
 
 * Building only azure-c-sdk, run build.azure-c-sdk.cmd
 * Building only limpet, run build.azure-dm.cmd

#### Setup Azure cloud resources

Setup cloud resources by following steps mentioned in [here](https://docs.microsoft.com/en-us/azure/iot-dps/tutorial-set-up-cloud) and gather the information below.

    --ID Scope - You can get from Azure portal -> Device Provisioning Services -> Overview -> ID Scope.
    --Global device end point - You can get from Azure portal -> Device Provisioning Services -> Overview -> Global device endpoint.  

#### Enroll the device in DPS
* Set up Windows IoT device with TPM by using the below link if you have not already.
    https://developer.microsoft.com/en-us/windows/iot/getstarted

* Connect to device using PowerShell by using device administrator credentials from your development machine.

* Copy the limpet tool (limpet.exe built in the previous step) to Windows IoT device.
* Run limpet tool from remote powershell connection.
    limpet.exe -azuredps -enrollmentinfo
    Tool prints endorsement key and registration id, please note down.
  
  Here is the screenshot for reference:
  
  <img src="limpetazuredpsenrollinfo.PNG"/>

* Enroll the device in DPS by following TPM based devices steps in the below link,
    https://docs.microsoft.com/en-us/azure/iot-dps/tutorial-provision-device-to-hub#enrolldevice

#### Register the device in IotHub using DPS.

* Run the below command in remote PowerShell connection to register the current device in IotHub using DPS service.

        limpet.exe -azuredps -register <tpm slot number> <global dps uri end point> <scope id>
          <tpm slot number>: tpm slot number to store the key.
          <global dps service uri>: it is generally fixed global.azure-devices-provisioning.net
          <scope id>: customer dps scope id. you can get this information from azure portal dps service ->overview ->id scope.


##### Verification

* Option 1: From the command line:

    	run "limpet <TPM_SLOT> -rur"
        It should display the service uri of the device, which confirms the device registered successfully in Azure IoT Hub.
	
    	run "limpet <TPM_SLOT> -ast"
        It should display the IotHub connection string of the device, which confirms the device registered successfully in Azure IoT Hub.

* Option 2: You can also use the Azure portal -> iothub device explorer and can find the device.

* Option 3: Use the [Device Management(DM) hello world application](<https://github.com/ms-iot/iot-core-azure-dm-client/blob/master/docs/dm-hello-world-overview.md>) for managing the device.

Here is the screen shot for reference(morphed PII info):

<img src="limpetazuredpsregister.png"/>


#### Setting up remote PowerShell connection
	Start the PowerShell by running as administrator in your development machine.
	$ip = "<Ip Address>"
	$password = "<administrator password>"
	$username = "administrator"
	$secstr = New-Object -TypeName System.Security.SecureString
	$password.ToCharArray() | ForEach-Object {$secstr.AppendChar($_)}
	$cred = new-object -typename System.Management.Automation.PSCredential -argumentlist $username, $secstr
	Set-Item -Path WSMan:\localhost\Client\TrustedHosts -Value "$ip" -Force
	$session = New-PSSession -ComputerName $ip -Credential $cred
	Enter-Pssession $session

    If you are having trouble using remote PowerShell connection see https://developer.microsoft.com/en-us/windows/iot/docs/powershell

#### Learn more Azure device provisioning service
* [Blog](<https://azure.microsoft.com/de-de/blog/azure-iot-hub-device-provisioning-service-preview-automates-device-connection-configuration/>)
* [Documentation](<https://docs.microsoft.com/en-us/azure/iot-dps/>)
* [Service SDK (preview release)](<https://www.nuget.org/packages/Microsoft.Azure.Devices/>)
* [Device SDK](<https://github.com/Azure/azure-iot-sdk-c>)

