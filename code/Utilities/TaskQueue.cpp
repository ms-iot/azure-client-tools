// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "TaskQueue.h"
#include "Logger.h"

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

TaskQueue::TaskQueue()
{
}

future<wstring> TaskQueue::Enqueue(Task task)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    future<wstring> response = task.get_future();

    unique_lock<mutex> l(_mutex);
    _queue.push(move(task));
    l.unlock();

    _cv.notify_one();

    return response;
}

TaskQueue::Task TaskQueue::Dequeue()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    unique_lock<mutex> l(_mutex);
    _cv.wait(l, [&] { return !_queue.empty(); });

    Task taskItem = move(_queue.front());
    _queue.pop();

    return taskItem;
}

}}}}
