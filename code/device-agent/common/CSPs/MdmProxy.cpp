// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <stdafx.h>
#include "..\Plugins\PluginConstants.h"
#include "MdmProxy.h"
#include "SyncMLHelpers.h"

#define ROOT_XML L"SyncML\\SyncBody"
#define ROOT_START_TAG L"<" ROOT_XML L">"
#define ROOT_END_TAG L"</" ROOT_XML L">"
#define STATUS_XML_PATH ROOT_XML L"\\Status\\Data\\"
#define RESULTS_XML_PATH ROOT_XML L"\\Results\\Item\\Data\\"

using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Common {

    MdmProxy::MdmProxy()
    {
    }

    void MdmProxy::SetMdmServer(std::shared_ptr<IMdmServer> iMdmServer)
    {
        _iMdmServer = iMdmServer;
    }

    void MdmProxy::RunSyncML(const string& inSyncML, string& outSyncML)
    {
        if (_iMdmServer == nullptr)
        {
            throw DMException(DMSubsystem::DeviceAgent, DM_MDM_ERROR_UNINITIALIZED_MDMSERVER, "MDM Server is not initialized");
        }

        outSyncML = _iMdmServer->RunSyncML("", inSyncML);
    }

    void MdmProxy::RunSet(const string& cspPath, const string& value)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Replace>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += cspPath;
        requestSyncML += R"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">text/plain</Type>
                </Meta>
                <Data>)";
        requestSyncML += value;
        requestSyncML += R"(</Data>
              </Item>
            </Replace>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);
    }

    void MdmProxy::RunSet(const string& cspPath, int value)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Replace>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += cspPath;
        requestSyncML += R"(</LocURI>
                </Target>
                <Meta><Format xmlns="syncml:metinf">int</Format></Meta>
                <Data>)";
        requestSyncML += to_string(value);
        requestSyncML += R"(</Data>
              </Item>
            </Replace>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);
    }

    void MdmProxy::RunSet(const string& path, bool value)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Replace>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += R"(</LocURI>
                </Target>
                <Meta><Format xmlns="syncml:metinf">bool</Format></Meta>
                <Data>)";
        requestSyncML += value ? "True" : "False";
        requestSyncML += R"(</Data>
              </Item>
            </Replace>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);
    }

    string MdmProxy::RunGetString(const string& path)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Get>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += R"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">text/plain</Type>
                </Meta>
              </Item>
            </Get>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);

        wstring wideResultSyncML = Utils::MultibyteToWide(resultSyncML.c_str());
        wstring wideValueString;
        ReadXmlValue(wideResultSyncML, RESULTS_XML_PATH, wideValueString);

        return Utils::WideToMultibyte(wideValueString.c_str());
    }

    string MdmProxy::RunGetBase64(const string& path)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Get>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += R"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">b64</Type>
                </Meta>
              </Item>
            </Get>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);

        wstring wideResultSyncML = Utils::MultibyteToWide(resultSyncML.c_str());
        wstring wideValueString;
        ReadXmlValue(wideResultSyncML, RESULTS_XML_PATH, wideValueString);
        return  Utils::WideToMultibyte(wideValueString.c_str());
    }

    unsigned int MdmProxy::RunGetUInt(const string& path)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Get>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path.c_str();
        requestSyncML += R"(</LocURI>
                </Target>
                <Meta>
                    <Format xmlns="syncml:metinf">int</Format>
                </Meta>
              </Item>
            </Get>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);

        // Extract the result data
        wstring wideResultSyncML = Utils::MultibyteToWide(resultSyncML.c_str());
        wstring wideValueString;
        ReadXmlValue(wideResultSyncML, RESULTS_XML_PATH, wideValueString);

        return stoi(wideValueString);
    }

    void MdmProxy::RunGetStructData(const std::string& path, ELEMENT_HANDLER handler)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Get>
              <CmdID>1</CmdID>
              <Item>
                <Target>
                  <LocURI>)";
        requestSyncML += path;
        requestSyncML += R"(</LocURI>
                </Target>
                <Meta>
                    <Type xmlns="syncml:metinf">text/plain</Type>
                </Meta>
              </Item>
            </Get>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);

        // Extract the result data
        ReadXmlStructData(Utils::MultibyteToWide(resultSyncML.c_str()), handler);
    }

    void MdmProxy::RunExec(const string& path)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Exec>
                <CmdID>1</CmdID>
                <Item>
                    <Target>
                        <LocURI>)";
            requestSyncML += path;
            requestSyncML += R"(</LocURI>
                    </Target>
                </Item>
            </Exec>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);
    }

    void MdmProxy::RunAddTyped(const string& path, const string& type)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Add>
                <CmdID>1</CmdID>
                <Item>
                    <Target>
                        <LocURI>)";
        requestSyncML += path;
        requestSyncML += R"(</LocURI>
                    </Target>
                    <Meta>
                        <Format xmlns="syncml:metinf">)";
        requestSyncML += type;
        requestSyncML += R"(</Format>
                    </Meta>
                </Item>
            </Add>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);
    }

    void MdmProxy::RunAdd(const string& path, const string& value)
    {
        string requestSyncML = R"(
        <SyncBody>
            <Add>
                <CmdID>1</CmdID>
                <Item>
                    <Target>
                        <LocURI>)";
        requestSyncML += path;
        requestSyncML += "/";
        requestSyncML += value;
        requestSyncML += R"(</LocURI>
                    </Target>
                </Item>
            </Add>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);
    }

    void MdmProxy::RunAddDataBase64(const string& path, const string& value)
    {
        RunAddData(path, value, "b64");
    }

    void MdmProxy::RunAddData(const string& path, const string& value, const string& type)
    {
        string requestSyncML = R"(
        <SyncBody>
        <Add>
        <CmdID>1</CmdID>
        <Item>
            <Target>
                <LocURI>)";
                requestSyncML += path;
                requestSyncML += R"(</LocURI>
            </Target>
            <Meta>
                <Format xmlns="syncml:metinf">)";
                requestSyncML += type;
                requestSyncML += R"(</Format>
            </Meta>
            <Data>)";
                requestSyncML += value;
                requestSyncML += R"(</Data>
        </Item>
        </Add>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);

    }

    void MdmProxy::RunDelete(const string& path)
    {
        string requestSyncML = R"(
        <SyncBody>
        <Delete>
        <CmdID>1</CmdID>
        <Item>
            <Target>
                <LocURI>)";
                requestSyncML += path;
                requestSyncML += R"(</LocURI>
            </Target>
        </Item>
        </Delete>
        </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);
    }

    void MdmProxy::RunExecWithParameters(const string& path, const string& params)
    {
        string requestSyncML = R"(
    <SyncBody>
        <Exec>
            <CmdID>1</CmdID>
            <Item>
                <Target>
                    <LocURI>)";
        requestSyncML += path;
        requestSyncML += R"(</LocURI>
                </Target>
                <Meta>
                    <Format xmlns="syncml:metinf">chr</Format>
                </Meta>
                <Data>)";
        requestSyncML += params;
        requestSyncML += R"(</Data>
            </Item>
        </Exec>
    </SyncBody>
        )";

        string resultSyncML;
        RunSyncML(requestSyncML, resultSyncML);
    }

}}}}
