# TPM Compatibility for DPS Scenarios

Below is a table of tested boards:

| Board | Version | Meets Azure TPM Requirements |
|-------|---------|---------|
| DragonBoard | 2.0 | Yes |
| RP2/3 | - | No |
| MBM   | 2.0 | No |

If your board does not show in the table above, here is how to determine the compatibility of your device:

1. Get Limpet.exe (see the [Installing Pre-built Binaries](../install-pre-built-binaries.md) page).

2. Run the following command

    <pre>
    C:\&gt; limpet.exe -gti
    &lt;?xml version="1.0" encoding="utf-8"?&gt;
    &lt;TpmInfo&gt;
      &lt;TpmType&gt;FirmwareTZ&lt;/TpmType&gt;
      &lt;TpmFamily&gt;2.0&lt;/TpmFamily&gt;
      &lt;TpmLevel&gt;0&lt;/TpmLevel&gt;
      &lt;TpmRevision&gt;0.94&lt;/TpmRevision&gt;
      &lt;TpmManufacturer&gt;QCOM&lt;/TpmManufacturer&gt;
      &lt;TpmVendorString&gt;xCG &lt;/TpmVendorString&gt;
      &lt;TpmFirmware&gt;2.1.0.1&lt;/TpmFirmware&gt;
      &lt;LockoutAuth&gt;Unavailable&lt;/LockoutAuth&gt;
      &lt;EndorsementAuth&gt;NotSet&lt;/EndorsementAuth&gt;
      &lt;StorageAuth&gt;NotSet&lt;/StorageAuth&gt;
      &lt;DisableClear&gt;false&lt;/DisableClear&gt;
      &lt;InLockout&gt;false&lt;/InLockout&gt;
      &lt;RSA&gt;true&lt;/RSA&gt;
      &lt;HMAC&gt;true&lt;/HMAC&gt;
      &lt;AES&gt;true&lt;/AES&gt;
      &lt;ECC&gt;false&lt;/ECC&gt;
      &lt;TcgLogSize&gt;53015&lt;/TcgLogSize&gt;
    &lt;/TpmInfo&gt;
    </pre>

Note the following fields:
- `TpmFamily` has to be `2.0` or higher.
- `HMAC` has to be `true`.


## Troubleshooting

If configuring the TPM is not successful for QCDB410C (Dragonboard), try following these steps to reset the TPM:

1. Unplug the board
2. Hold down the (|) and (+) button and power the board
3. Move the cursor with (-) and (+) to select "15. UEFI Menu" and confirm with (|)
4. Move the cursor with (-) and (+) to select "5. Clear UEFI BS Variables" and confirm with (|)
5. Move the cursor with (-) and (+) to select "6. Clear UEFI RT Variables and fTPM (Erase RPMB)" and confirm with (|)
6. Unplug the board


