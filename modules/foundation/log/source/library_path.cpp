/**************************************************************************/
/* library_path.cpp                                                       */
/**************************************************************************/
/*                          This file is part of:                         */
/*                                SushiLog                                */
/*                https://github.com/SushiSystems/SushiLog                */
/*                         https://sushisystems.io                        */
/**************************************************************************/
/* Copyright (c) 2026-present Mustafa Garip & Sushi Systems               */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License"); you    */
/* may not use this file except in compliance with the License. You may   */
/* obtain a copy of the License at                                        */
/*                                                                        */
/*     https://www.apache.org/licenses/LICENSE-2.0                        */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/**************************************************************************/

/**
 * @file library_path.cpp
 * @brief Defines library_path, the file the SushiLog code of this process was loaded from.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "library_path.hpp"

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace SushiLog::Detail
{
    std::string library_path()
    {
        #ifdef _WIN32
            HMODULE module = nullptr;
            const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
            if (GetModuleHandleExA(flags, reinterpret_cast<LPCSTR>(&library_path), &module) == 0)
                return "(unknown)";
            char buffer[MAX_PATH];
            const DWORD size = GetModuleFileNameA(module, buffer, MAX_PATH);
            if (size == 0)
                return "(unknown)";
            return std::string(buffer, size);
        #else
            Dl_info info{};
            if (dladdr(reinterpret_cast<void*>(&library_path), &info) == 0 ||
                info.dli_fname == nullptr)
                return "(unknown)";
            return info.dli_fname;
        #endif
    }
} // namespace SushiLog::Detail
