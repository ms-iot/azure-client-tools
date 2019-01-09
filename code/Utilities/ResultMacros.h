// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// Win32 specific error macros

#define FAILED_WIN32(win32err)                              ((win32err) != 0)
#define SUCCEEDED_WIN32(win32err)                           ((win32err) == 0)

// Conditionally returns failures (HRESULT)
#define RETURN_HR_IF(hr, condition)                             do { if (condition) { return hr; }} while (0, 0)
#define RETURN_HR_IF_NULL(hr, ptr)                              do { if ((ptr) == nullptr) { return hr;}} while(0,0)
#define RETURN_HR_IF_FAILED(hr)                                 do { if (FAILED(hr)) { return hr;}} while(0,0)
#define RETURN_HR(hr) return hr

#define FAIL_FAST_IF_FAILED(hr, msg) do { if(hr != S_OK) {throw hr; } } while(0,0)

#ifdef max
#undef max
#endif

template<typename T> T from_sizet(size_t in) {
#pragma warning(push)
#pragma warning(disable:4127)
    if (sizeof(T) < sizeof(size_t) && in > static_cast<size_t>(std::numeric_limits<T>::max())) {
#pragma warning(pop)
        throw std::overflow_error("size_t val > destination type max");
    }
    return static_cast<T>(in);
}