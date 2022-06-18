//--------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#pragma once

#include <cstdio>
#include <exception>

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr, const char* _f, const size_t _l, const char* _fu)
            : result(hr), file(_f), line(_l), func(_fu) {}

        const char* what() const override
        {
            static char s_str[198] = {};
            sprintf_s(s_str, "File: %s\nLine: %zu\nFunction: %s\nFailure with HRESULT of %08X",
                file, line, func, static_cast<unsigned int>(result));

            return s_str;
        }

    private:
        HRESULT result;
        const char* file;
        const size_t line;
        const char* func;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr, const char* file, const size_t line, const char* func)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr, file, line, func);
        }
    }
}

#define HR(hr) DX::ThrowIfFailed(hr, __FILE__, __LINE__, __func__);