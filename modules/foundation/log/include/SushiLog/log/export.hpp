/**************************************************************************/
/* export.hpp                                                             */
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
 * @file export.hpp
 * @brief Defines SUSHILOG_API, which exports a symbol from the shared library.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

/**
 * @brief Marks a function the library defines and a consumer calls.
 *
 * The static target defines SUSHILOG_STATIC, which leaves every symbol hidden in the binary
 * that links it. The shared library is built with SUSHILOG_EXPORTS.
 */
#if defined(SUSHILOG_STATIC)
    #define SUSHILOG_API
#elif defined(_WIN32) || defined(__CYGWIN__)
    #if defined(SUSHILOG_EXPORTS)
        #define SUSHILOG_API __declspec(dllexport)
    #else
        #define SUSHILOG_API __declspec(dllimport)
    #endif
#else
    #define SUSHILOG_API __attribute__((visibility("default")))
#endif
