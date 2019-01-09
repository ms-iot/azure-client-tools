// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma prefast(push)
#pragma prefast(disable: 28718, "Certpol.h has an unannotated buffer")
#pragma prefast(disable: 28285, "Objidlbase.h has an annotation syntax error")

#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <Strsafe.h>
#include <Windows.h>
#include <BCrypt.h>
#include <NCrypt.h>
#include <Wincrypt.h>
#include <CertEnroll.h>
#include <tbs.h>
#include "UrchinLib.h"
#include "UrchinPlatform.h"
#define LIMPETEXE 1
#include "LimpetApi.h"
#include "resources.h"
#include "..\Utilities\AutoCloseHandle.h"
#include "..\Utilities\AutoCloseTypes.h"
#pragma prefast(pop)

