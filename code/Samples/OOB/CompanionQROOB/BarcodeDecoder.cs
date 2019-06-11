// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Text;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;

namespace CompanionQROOB
{
    class BarcodeDecoder
    {
        static char[] _ascii = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                             ' ', '!', '"', '#', '$', '%', '&', '\'','(', ')', '*', '+', ',', '-', '.', '/',
                             '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ',', '<', '=', '>', '?',
                             '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'o',
                             'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^','_',
                             '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
                             'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', ' '
                           };

        public static string DecodeLabel(IBuffer scanDataLabel)
        {
            string text = CryptographicBuffer.EncodeToHexString(scanDataLabel);
            StringBuilder decodedText = new StringBuilder();
            for (int i = 0; i < text.Length - 1; i += 2)
            {
                StringBuilder t = new StringBuilder();
                t.Append(text[i]);
                t.Append(text[i + 1]);
                int asciiCode = Int32.Parse(t.ToString(), System.Globalization.NumberStyles.HexNumber);
                decodedText.Append(_ascii[asciiCode]);
            }

            return decodedText.ToString();
        }
    }
}
