// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>

namespace Microsoft { namespace Azure { namespace DeviceManagement { namespace Utils {

    template<class T>
    class AutoCloseBase
    {
    public:

        AutoCloseBase(T&& handle, const std::function<BOOL(T)>& cleanUp) :
            _handle(handle),
            _cleanUp(cleanUp)
        {
            handle = NULL;
        }

        void SetHandle(T&& handle)
        {
            _handle = handle;
            handle = NULL;
        }

        void SetHandle(T& handle)
        {
            if (_handle != NULL)
            {
                Close();
            }
            _handle = handle;
            handle = NULL;
        }

        T Get() { return _handle; }
        uint64_t Get64() { return reinterpret_cast<uint64_t>(_handle); }
        T* GetAddress() { return &_handle; }

        // We're following the ComPtr pattern and that & is assumed to be an indication of a write that's about to happen.
        // Reference: https://docs.microsoft.com/en-us/cpp/windows/comptr-operator-ampersand-operator
        T* operator&()
        {
            if (_handle != NULL)
            {
                Close();
            }
            return &_handle;
        }

        T& GetReference()
        {
            return _handle;
        }

        BOOL Close()
        {
            BOOL result = TRUE;
            if (_handle != NULL && _cleanUp)
            {
                result = _cleanUp(_handle);
                _handle = NULL;
            }
            return result;
        }

        virtual ~AutoCloseBase()
        {
            Close();
        }

    private:
        AutoCloseBase(const AutoCloseBase &);            // prevent copy
        AutoCloseBase& operator=(const AutoCloseBase&);  // prevent assignment

        T _handle;
        std::function<BOOL(T)> _cleanUp;
    };

}}}}
