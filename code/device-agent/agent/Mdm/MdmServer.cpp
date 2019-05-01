// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "MdmServer.h"

#include "WinSDKRS2.h"

#define ROOT_XML L"SyncML\\SyncBody"
#define ROOT_START_TAG L"<" ROOT_XML L">"
#define ROOT_END_TAG L"</" ROOT_XML L">"
#define STATUS_XML_PATH ROOT_XML L"\\Status\\Data\\"
#define RESULTS_XML_PATH ROOT_XML L"\\Results\\Item\\Data\\"

using namespace std;
using namespace DMCommon;
using namespace DMUtils;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Client {

    shared_ptr<MdmServer> MdmServer::_this;
    recursive_mutex MdmServer::_lock;

    class SyncMLServer
    {
    public:
        SyncMLServer()
        {
            TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
            _workerThread = thread(ServiceWorkerThread, this);
        }

        void Process(const wstring& requestSyncML, wstring& outputSyncML)
        {
            TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
            TaskQueue::Task task([&]()
            {
                return ProcessInternal(requestSyncML);
            });

            future<wstring> futureResult = _queue.Enqueue(move(task));

            // This will block until the result is available...
            outputSyncML = futureResult.get();
        }

    private:

        static void ServiceWorkerThread(void* context)
        {
            TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

            SyncMLServer* syncMLServer = static_cast<SyncMLServer*>(context);
            syncMLServer->Listen();
        }

        void Listen()
        {
            TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

            while (true)
            {
                TRACELINE(LoggingLevel::Verbose, "Worker thread waiting for a task to be queued...");
                TaskQueue::Task task = _queue.Dequeue();

                TRACELINE(LoggingLevel::Verbose, "A task has been dequeued...");
                task();

                TRACELINE(LoggingLevel::Verbose, "Task has completed.");
            }
        }

        wstring ProcessInternal(const wstring& requestSyncML)
        {
            TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

            TRACELINEP(LoggingLevel::Verbose, L"SyncMLServer - Request : ", requestSyncML.c_str());

            HRESULT hr = RegisterDeviceWithLocalManagement(NULL);
            if (FAILED(hr))
            {
                throw DMException(DMSubsystem::MDM, hr, "RegisterDeviceWithLocalManagement");
            }

            PWSTR output = nullptr;
            hr = ApplyLocalManagementSyncML(requestSyncML.c_str(), &output);
            if (FAILED(hr))
            {
                throw DMException(DMSubsystem::MDMOperation, hr, "ApplyLocalManagementSyncML");
            }

            wstring outputSyncML;
            if (output)
            {
                outputSyncML = output;
            }
            LocalFree(output);

            return outputSyncML;
        }

        thread _workerThread;
        TaskQueue _queue;
    };

    MdmServer::MdmServer()
    {}

    shared_ptr<IMdmServer> MdmServer::GetInstance()
    {
        LockGuard lk(&_lock);

        if (_this == nullptr)
        {
            _this = make_shared<MdmServer>();
        }

        return _this;
    }

    std::string MdmServer::RunSyncML(const std::string& sid, const std::string& syncML)
    {
        LockGuard lk(&_lock);

        wstring wideSID = Utils::MultibyteToWide(sid.c_str());
        wstring wideInSyncML = Utils::MultibyteToWide(syncML.c_str());
        wstring wideOutSyncML;

        static SyncMLServer syncMLServer;
        syncMLServer.Process(wideInSyncML, wideOutSyncML);

        TRACELINEP(LoggingLevel::Verbose, L"Response: ", wideOutSyncML.c_str());

        vector<unsigned int> returnCodes;
        ReadXmlStatus(wideOutSyncML, STATUS_XML_PATH, returnCodes);
        for (unsigned int code : returnCodes)
        {
            if (code >= 300)
            {
                TRACELINE(LoggingLevel::Verbose, L"Error:\n\n");
                TRACELINEP(LoggingLevel::Verbose, L"Request:\n", wideInSyncML.c_str());
                TRACELINEP(LoggingLevel::Verbose, L"Response:\n", wideOutSyncML.c_str());
                TRACELINEP(LoggingLevel::Verbose, L"Error:\n", code);
                throw DMException(DMSubsystem::MDM, code, "Error in processing syncML");
                break;
            }
        }


        return Utils::WideToMultibyte(wideOutSyncML.c_str());
    }

}}}}
