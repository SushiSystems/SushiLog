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
/* Licensed under the PolyForm Noncommercial License 1.0.0 (the           */
/* "License"); you may not use this file except in compliance with the    */
/* License. You may obtain a copy of the License at                       */
/*                                                                        */
/*     https://polyformproject.org/licenses/noncommercial/1.0.0           */
/*                                                                        */
/* Noncommercial use is free. Commercial use requires a separate licence  */
/* from Sushi Systems; see COMMERCIAL.md. The software is provided        */
/* "as is", without warranty of any kind.                                 */
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
