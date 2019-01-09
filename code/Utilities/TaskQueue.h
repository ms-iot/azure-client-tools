// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <assert.h>
#include <mutex>
#include <future>
#include <queue>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class LockGuard
    {
    public:
        LockGuard(std::mutex* m) :
            _m(m)
        {
            assert(m != nullptr);

            _m->lock();
        }
        ~LockGuard()
        {
            _m->unlock();
        }
    private:
        std::mutex* _m;
    };

    class TaskQueue
    {
    public:
        typedef std::packaged_task<std::wstring()> Task;

        TaskQueue();

        std::future<std::wstring> Enqueue(Task task);
        Task Dequeue();

    private:
        std::queue<Task> _queue;

        std::mutex _mutex;
        std::condition_variable _cv;
    };

}}}}
