// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.IO.Compression;

namespace ZipHelperUtility
{
    class Program
    {
        static void Main(string[] args)
        {
            bool showHelp = false;

            if (args.Length != 2)
            {
                showHelp = true;

                if (args.Length != 1 || (args[0] != "-h" && args[0] != "-?" && args[0] != "/h" && args[0] != "/?"))
                {
                    Console.WriteLine();
                    Console.WriteLine("Error: Invalid parameters.");
                }
            }

            if (showHelp)
            {
                ShowHelp();
                return;
            }

            string inputFolder = args[0];
            string outputFileName = args[1];

            if (!Directory.Exists(inputFolder))
            {
                Console.WriteLine("Error: Input folder does not exist.");
                return;
            }

            if (File.Exists(outputFileName))
            {
                Console.WriteLine("Target file already exists. Deleting...");
                File.Delete(outputFileName);
            }

            try
            {
                Console.WriteLine("Input folder: " + inputFolder);
                Console.WriteLine("Output file : " + outputFileName);
                Console.WriteLine("Compressing...");
                ZipFile.CreateFromDirectory(inputFolder, outputFileName);
                Console.WriteLine("Done.");
            }
            catch (Exception e)
            {
                Console.WriteLine("Error: " + e.Message);
            }
        }

        static void ShowHelp()
        {
            Console.WriteLine();
            Console.WriteLine("Usage:");
            Console.WriteLine();
            Console.WriteLine("ZipHelperUtility -h");
            Console.WriteLine("ZipHelperUtility /h");
            Console.WriteLine("ZipHelperUtility input-folder-to-zip output-file-name.zip");
        }
    }
}
