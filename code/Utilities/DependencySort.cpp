// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DependencySort.h"
#include "DMException.h"

using namespace std;

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    DependencySort::DependencySort()
    {
    }

    void DependencySort::SetDependenciesMap(string handler, vector<string> dependency)
    { 
        for(auto& a : dependency)
        {
           _dMap[a].push_back(handler);
        }
        _dependencyMap[handler] = dependency;
    }

    // The function to do Topological Sort. 
    vector<string> DependencySort::SortDependency()
    {
        // Create a map to store number of dependencies a handler has.
        map<string, int> dependencies;
        for (auto& v : _dependencyMap)
        {
            dependencies[v.first] = v.second.size();
        }

        // Create an queue and enqueue all handlers with zero dependencies
        queue<string> q;
        for (auto& i : dependencies)
        {
            if (dependencies[i.first] == 0)
            {
                q.push(i.first);
            }
        }

        // Initialize count of visited handlers 
        int cnt = 0;

        // Create a vector to store result (A topological ordering of the handlers) 
        vector <string> result;

        // One by one dequeue handlers from queue and enqueue adjacents if adjacent handlers dependencies becomes 0 
        while (!q.empty())
        {
            // dequeue and add it to topological ordered result 
            string u = q.front();
            q.pop();
            result.push_back(u);

            // Iterate through all its neighbouring nodes of dequeued node u and decrease their dependencies by 1 
            vector<string>::iterator itr;
            for (itr = _dMap[u].begin(); itr != _dMap[u].end(); itr++)
            {
                // If dependencies becomes zero, add it to queue 
                if (--dependencies[*itr] == 0)
                {
                    q.push(*itr);
                }
            }

            cnt++;
        }

        // Check if there was a cycle 
        if (cnt != _dependencyMap.size())
        {
            throw DMException(DMSubsystem::DeviceAgentPlugin, DM_ERROR_PLUGIN_HANDLER_INVALID_DEPENDENCY, "Circular dependency found in the handlers.");
        }

        return result;
    }
}}}}