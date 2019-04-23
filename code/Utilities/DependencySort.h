// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include <list>
#include <vector>
#include <map>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    class DependencySort
    {
        std::map<std::string, std::vector<std::string>> _dependencyMap; //handler to dependency maping
        std::map<std::string, std::vector<std::string>> _dMap; // handler to dependents maping

    public:
        DependencySort();
 
        void SetDependenciesMap(std::string handler, std::vector<std::string> dependency);

        std::vector<std::string> SortDependency();
    };

}}}}