// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "JsonUtils.h"
#include "Logger.h"
#include "DMException.h"

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    Json::Value JsonUtils::JsonObjectFromString(const std::string& stringValue)
    {
        istringstream payloadStream(stringValue);
        Json::Value root;
        string errorsList;
        Json::CharReaderBuilder builder;
        if (!Json::parseFromStream(builder, payloadStream, &root, &errorsList))
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_ERROR_INVALID_JSON, errorsList);
        }
        return root;
    }

    Json::Value JsonUtils::ParseJSONFile(const string& fileName)
    {
        TRACELINEP(LoggingLevel::Verbose, "Parsing json file: ", fileName.c_str());

        if (fileName.length() == 0)
        {
            TRACELINE(LoggingLevel::Error, "File name cannot be empty.");
            throw invalid_argument("No file path set");
        }

        Json::CharReaderBuilder builder;
        ifstream jsonIfStream(fileName, ifstream::binary);
        if (!jsonIfStream.good())
        {
            TRACELINE(LoggingLevel::Error, "Failed to open file.");
            throw runtime_error("Failed to open file");
        }
        string errs;
        Json::Value root;

        if (!Json::parseFromStream(builder, jsonIfStream, &root, &errs))
        {
            TRACELINE(LoggingLevel::Error, "Failed to parse json file content.");
            Json::throwRuntimeError(errs);
        }

        return root;
    }

    bool JsonUtils::TryParseJSONFile(const string& fileName, Json::Value& readJson)
    {
        TRACELINEP(LoggingLevel::Verbose, "Trying to parse json file: ", fileName.c_str());

        bool result = false;
        try
        {
            readJson = ParseJSONFile(fileName);
            result = true;
        }
        catch (const exception& ex)
        {
            string msg = "Could not open json file: ";
            msg += fileName;
            msg += ". Error: ";
            msg += ex.what();
            TRACELINE(LoggingLevel::Warning, msg.c_str());
        }
        return result;
    }

    void JsonUtils::WriteJSONFile(const string& fileName, Json::Value& json)
    {
        TRACELINEP(LoggingLevel::Verbose, "Parsing json file: ", fileName.c_str());

        if (fileName.length() == 0)
        {
            TRACELINE(LoggingLevel::Error, "File name cannot be empty.");
            throw invalid_argument("No file path set");
        }

        ofstream jsonOfStream(fileName, ofstream::binary);
        if (!jsonOfStream.good())
        {
            TRACELINE(LoggingLevel::Error, "Failed to open file.");
            throw runtime_error("Failed to open file");
        }

        Json::StreamWriterBuilder builder;
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        writer->write(json, &jsonOfStream);
    }

    bool JsonUtils::TryWriteJSONFile(const string& fileName, Json::Value& json)
    {
        TRACELINEP(LoggingLevel::Verbose, "Trying to write json file: ", fileName.c_str());

        bool result = false;
        try
        {
            WriteJSONFile(fileName, json);
            result = true;
        }
        catch (const exception& ex)
        {
            string msg = "Could not open json file: ";
            msg += fileName;
            msg += ". Error: ";
            msg += ex.what();
            TRACELINE(LoggingLevel::Warning, msg.c_str());
        }
        return result;
    }

}}}}
