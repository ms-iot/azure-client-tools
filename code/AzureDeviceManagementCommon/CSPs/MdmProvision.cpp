// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "MdmProvision.h"

#define ROOT_XML L"SyncML\\SyncBody"
#define ROOT_START_TAG L"<" ROOT_XML L">"
#define ROOT_END_TAG L"</" ROOT_XML L">"
#define STATUS_XML_PATH ROOT_XML L"\\Status\\Data\\"
#define RESULTS_XML_PATH ROOT_XML L"\\Results\\Item\\Data\\"

using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    void MdmProvision::RunSyncML(const wstring&, const wstring& requestSyncML, wstring& outputSyncML)
    {
        throw DMException(-1, "legacy!!");
    }

    void MdmProvision::RunAdd(const wstring& sid, const wstring& path, const wstring& value)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Add>
                <CmdID>1</CmdID>
                <Item>
                    <Target>
                        <LocURI>)";
        requestSyncML += path;
        requestSyncML += L"/";
        requestSyncML += value;
        requestSyncML += LR"(</LocURI>
                    </Target>
                </Item>
            </Add>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunAddData(const wstring& sid, const wstring& path, const wstring& value, const wstring& type)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Add>
                <CmdID>1</CmdID>
                <Item>
                    <Target>
                        <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                    </Target>
                    <Meta>
                        <Format xmlns="syncml:metinf">)";
        requestSyncML += type;
        requestSyncML += LR"(</Format>
                    </Meta>
                    <Data>)";
        requestSyncML += value;
        requestSyncML += LR"(</Data>
                </Item>
            </Add>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunAddTyped(const wstring& sid, const wstring& path, const wstring& type)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Add>
                <CmdID>1</CmdID>
                <Item>
                    <Target>
                        <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                    </Target>
                    <Meta>
                        <Format xmlns="syncml:metinf">)";
        requestSyncML += type;
        requestSyncML += LR"(</Format>
                    </Meta>
                </Item>
            </Add>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunAddDataBase64(const wstring& sid, const std::wstring& path, const std::wstring& value)
    {
        RunAddData(sid, path, value, L"b64");
    }

    void MdmProvision::RunDelete(const std::wstring& sid, const std::wstring& path)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Delete>
                <CmdID>1</CmdID>
                <Item>
                    <Target>
                        <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                    </Target>
                </Item>
            </Delete>
        </SyncBody>
        )";

        TRACELINEP(LoggingLevel::Verbose, L"RunDelete : ", requestSyncML.c_str());

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    wstring MdmProvision::RunGetString(const wstring& sid, const wstring& path)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Get>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">text/plain</Type>
                </Meta>
              </Item>
            </Get>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);

        wstring value;
        ReadXmlValue(resultSyncML, RESULTS_XML_PATH, value);
        return value;
    }

    std::wstring MdmProvision::RunGetBase64(const std::wstring& sid, const std::wstring& path)
    {
        // http://www.openmobilealliance.org/tech/affiliates/syncml/syncml_metinf_v101_20010615.pdf
        // Section 5.3.

        wstring requestSyncML = LR"(
        <SyncBody>
            <Get>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">b64</Type>
                </Meta>
              </Item>
            </Get>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);

        wstring value;
        ReadXmlValue(resultSyncML, RESULTS_XML_PATH, value);
        return value;
    }

    void MdmProvision::RunGetStructData(const std::wstring& path, ELEMENT_HANDLER handler)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Get>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">text/plain</Type>
                </Meta>
              </Item>
            </Get>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(L"", requestSyncML, resultSyncML);

        // Extract the result data
        ReadXmlStructData(resultSyncML, handler);
    }

    unsigned int MdmProvision::RunGetUInt(const wstring& sid, const wstring& path)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Get>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path.c_str();
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta>
                    <Format xmlns="syncml:metinf">int</Format>
                </Meta>
              </Item>
            </Get>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);

        // Extract the result data
        wstring valueString;
        ReadXmlValue(resultSyncML, RESULTS_XML_PATH, valueString);
        return stoi(valueString);
    }

    bool MdmProvision::RunGetBool(const wstring& sid, const wstring& path)
    {
        wstring result = RunGetString(sid, path);

        return 0 == _wcsicmp(result.c_str(), L"true");
    }

    bool MdmProvision::TryGetBool(const wstring& path, bool& value)
    {
        bool success = true;
        try
        {
            value = RunGetBool(L"", path);
        }
        catch (DMException& e)
        {
            success = false;
            TRACELINEP(LoggingLevel::Verbose, L"Error: GetBool() - path     : ", path.c_str());
            TRACELINEP(LoggingLevel::Verbose, "Error: GetBool() - exception: ", e.what());
        }
        return success;
    }

    void MdmProvision::RunSet(const wstring& sid, const wstring& path, const wstring& value)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Replace>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">text/plain</Type>
                </Meta>
                <Data>)";
        requestSyncML += value;
        requestSyncML += LR"(</Data>
              </Item>
            </Replace>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunSetBase64(const wstring& sid, const std::wstring& path, const std::wstring& value)
    {
        // http://www.openmobilealliance.org/tech/affiliates/syncml/syncml_metinf_v101_20010615.pdf
        // Section 5.3.

        wstring requestSyncML = LR"(
        <SyncBody>
            <Replace>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">b64</Type>
                </Meta>
                <Data>)";
        requestSyncML += value;
        requestSyncML += LR"(</Data>
              </Item>
            </Replace>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunSet(const wstring& sid, const wstring& path, int value)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Replace>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta><Format xmlns="syncml:metinf">int</Format></Meta>
                <Data>)";
        requestSyncML += to_wstring(value);
        requestSyncML += LR"(</Data>
              </Item>
            </Replace>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunSet(const wstring& sid, const wstring& path, bool value)
    {
        wstring requestSyncML = LR"(
        <SyncBody>
            <Replace>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta><Format xmlns="syncml:metinf">bool</Format></Meta>
                <Data>)";
        requestSyncML += value ? L"True" : L"False";
        requestSyncML += LR"(</Data>
              </Item>
            </Replace>
        </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunExec(const wstring& sid, const wstring& path)
    {
        wstring requestSyncML = LR"(
    <SyncBody>
        <Exec>
            <CmdID>1</CmdID>
            <Item>
                <Target>
                    <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
            </Item>
        </Exec>
    </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunExecWithParameters(const wstring& sid, const wstring& path, const wstring& params)
    {
        wstring requestSyncML = LR"(
    <SyncBody>
        <Exec>
            <CmdID>1</CmdID>
            <Item>
                <Target>
                    <LocURI>)";
        requestSyncML += path;
        requestSyncML += LR"(</LocURI>
                </Target>
                <Meta>
                    <Format xmlns="syncml:metinf">chr</Format>
                </Meta>
                <Data>)";
        requestSyncML += params;
        requestSyncML += LR"(</Data>
            </Item>
        </Exec>
    </SyncBody>
        )";

        wstring resultSyncML;
        RunSyncML(sid, requestSyncML, resultSyncML);
    }

    void MdmProvision::RunAdd(const wstring& path, const wstring& value)
    {
        // empty sid is okay for device-wide CSPs.
        RunAdd(L"", path, value);
    }

    void MdmProvision::RunAddData(const std::wstring& path, int value)
    {
        // empty sid is okay for device-wide CSPs.
        RunAddData(L"", path, Utils::MultibyteToWide(to_string(value).c_str()), L"int");
    }

    void MdmProvision::RunAddTyped(const wstring& path, const wstring& type)
    {
        RunAddTyped(L"", path, type);
    }

    void MdmProvision::RunAddData(const std::wstring& path, bool value)
    {
        // empty sid is okay for device-wide CSPs.
        RunAddData(L"", path, Utils::MultibyteToWide(to_string(value).c_str()), L"bool");
    }

    void MdmProvision::RunAddData(const wstring& path, const wstring& value)
    {
        // empty sid is okay for device-wide CSPs.
        RunAddData(L"", path, value);
    }

    void MdmProvision::RunAddDataBase64(const wstring& path, const wstring& value)
    {
        // empty sid is okay for device-wide CSPs.
        RunAddDataBase64(L"", path, value);
    }

    void MdmProvision::RunDelete(const std::wstring& path)
    {
        // empty sid is okay for device-wide CSPs.
        RunDelete(L"", path);
    }

    wstring MdmProvision::RunGetString(const wstring& path)
    {
        // empty sid is okay for device-wide CSPs.
        return RunGetString(L"", path);
    }

    bool MdmProvision::TryGetString(const wstring& path, wstring& value)
    {
        bool success = true;
        try
        {
            value = RunGetString(path);
        }
        catch (DMException& e)
        {
            success = false;
            TRACELINEP(LoggingLevel::Verbose, L"Error: GetString() - path     : ", path.c_str());
            TRACELINEP(LoggingLevel::Verbose, "Error: GetString() - exception: ", e.what());
        }
        return success;
    }

    wstring MdmProvision::RunGetBase64(const wstring& path)
    {
        // empty sid is okay for device-wide CSPs.
        return RunGetBase64(L"", path);
    }

    unsigned int MdmProvision::RunGetUInt(const wstring& path)
    {
        // empty sid is okay for device-wide CSPs.
        return RunGetUInt(L"", path);
    }

    bool MdmProvision::RunGetBool(const wstring& path)
    {
        // empty sid is okay for device-wide CSPs.
        return RunGetBool(L"", path);
    }

    void MdmProvision::RunSet(const wstring& path, const wstring& value)
    {
        // empty sid is okay for device-wide CSPs.
        RunSet(L"", path, value);
    }

    void MdmProvision::RunSet(const wstring& path, int value)
    {
        // empty sid is okay for device-wide CSPs.
        RunSet(L"", path, value);
    }

    void MdmProvision::RunSet(const wstring& path, bool value)
    {
        // empty sid is okay for device-wide CSPs.
        RunSet(L"", path, value);
    }

    void MdmProvision::RunSetBase64(const std::wstring& path, const std::wstring& value)
    {
        // empty sid is okay for device-wide CSPs.
        RunSetBase64(L"", path, value);
    }

    void MdmProvision::RunExec(const wstring& path)
    {
        // empty sid is okay for device-wide CSPs.
        RunExec(L"", path);
    }

    void MdmProvision::RunExecWithParameters(const std::wstring& path, const std::wstring& params)
    {
        RunExecWithParameters(L"", path, params);
    }

    void MdmProvision::ReportError(const wstring& syncMLRequest, const wstring& syncMLResponse, int errorCode)
    {
        TRACELINE(LoggingLevel::Verbose, L"Error:\n\n");
        TRACELINEP(LoggingLevel::Verbose, L"Request:\n", syncMLRequest.c_str());
        TRACELINEP(LoggingLevel::Verbose, L"Response:\n", syncMLResponse.c_str());
        TRACELINEP(LoggingLevel::Verbose, L"Error:\n", errorCode);
    }

    void MdmProvision::ReportError(const wstring& syncMLRequest, const wstring& syncMLResponse)
    {
        TRACELINE(LoggingLevel::Verbose, L"Error:\n\n");
        TRACELINEP(LoggingLevel::Verbose, L"Request:\n", syncMLRequest.c_str());
        TRACELINEP(LoggingLevel::Verbose, L"Response:\n", syncMLResponse.c_str());
    }


}}}}
