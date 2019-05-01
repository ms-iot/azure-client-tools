# Installing TPM Simulator

Note: This is supported only for the Windows IoT Core sku.

1. Get Limpet.exe (see the [Installing Pre-built Binaries](../install-pre-built-binaries.md) page).

2. To install the TPM simulator, run the following:
<pre>
    Limpet -IAT 1
</pre>

If you re-run `limpet.exe -gti`, you should see:

<pre>
[10.137.188.74]: PS C:\&gt; limpet.exe -gti
&lt;?xml version="1.0" encoding="utf-8"?&gt;
&lt;TpmInfo&gt;
  &lt;TpmType&gt;Emulator(NoSecurity)&lt;/TpmType&gt;
  &lt;TpmFamily&gt;2.0&lt;/TpmFamily&gt;
  &lt;TpmLevel&gt;0&lt;/TpmLevel&gt;
  &lt;TpmRevision&gt;1.38&lt;/TpmRevision&gt;
  &lt;TpmManufacturer&gt;MSFT&lt;/TpmManufacturer&gt;
  &lt;TpmVendorString&gt;TPM Simulator   &lt;/TpmVendorString&gt;
  &lt;TpmFirmware&gt;8215.531.17.21504&lt;/TpmFirmware&gt;
  &lt;LockoutAuth&gt;Available&lt;/LockoutAuth&gt;
  &lt;EndorsementAuth&gt;NotSet&lt;/EndorsementAuth&gt;
  &lt;StorageAuth&gt;NotSet&lt;/StorageAuth&gt;
  &lt;DisableClear&gt;false&lt;/DisableClear&gt;
  &lt;InLockout&gt;false&lt;/InLockout&gt;
  &lt;RSA&gt;true&lt;/RSA&gt;
  &lt;HMAC&gt;true&lt;/HMAC&gt;
  &lt;AES&gt;true&lt;/AES&gt;
  &lt;ECC&gt;true&lt;/ECC&gt;
  &lt;TcgLogSize&gt;32336&lt;/TcgLogSize&gt;
&lt;/TpmInfo&gt;
</pre>