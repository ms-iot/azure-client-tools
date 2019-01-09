// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "common_pch.h"
#include <filesystem>
#include <regex>
#include <atomic>
#include <exception>
#include <deque>
// NB <future> incompatible with pplxtasks when building in the windows tree
//#include <future>
#include <string>
#include <thread>
#include <vector>
#include <limits>
// todo: #include <pplx\pplxtasks.h>
#include <algorithm>
#include <string.h>
#include <Strsafe.h>
#include <map>
#include <sstream>

//#include "winsock2.h"

#include <windows.foundation.h>
#include <winhttp.h>
#include <wrl\internal.h>
#include <wrl.h>
#include <netlistmgr.h>
// todo: #include <cpprest\http_client.h>
// mime type names
// todo: #include <cpprest\details\http_helpers.h>

