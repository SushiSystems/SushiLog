/**************************************************************************/
/* sample_front.hpp                                                       */
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
 * @file sample_front.hpp
 * @brief Defines SAMPLE_LOG_*, a front as a Sushi project writes one, for the tests to log through.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#pragma once

#include <SushiLog/log/log_front.hpp>
#include <SushiLog/log/log_level.hpp>

#ifndef SAMPLE_ENABLE_LOGGING
    #define SAMPLE_ENABLE_LOGGING 1
#endif

namespace SushiLogTests
{
    /** @brief Holds the ceiling of this translation unit, as a project's front computes it. */
    [[maybe_unused]] constexpr SushiLog::LogLevel K_SAMPLE_LOG_CEILING =
        SushiLog::log_level_ceiling(SAMPLE_ENABLE_LOGGING != 0);
} // namespace SushiLogTests

/** @brief Logs at @p level in @p category through this translation unit's ceiling. */
#define SAMPLE_LOG_AT(level, category, ...)                                                      \
    SL_LOG_FRONT_AT(SushiLogTests::K_SAMPLE_LOG_CEILING, level, category, __VA_ARGS__)

/** @brief Logs step-by-step flow. */
#define SAMPLE_LOG_TRACE(category, ...)                                                          \
    SAMPLE_LOG_AT(SushiLog::LogLevel::Trace, category, __VA_ARGS__)

/** @brief Logs internal state. */
#define SAMPLE_LOG_DEBUG(category, ...)                                                          \
    SAMPLE_LOG_AT(SushiLog::LogLevel::Debug, category, __VA_ARGS__)

/** @brief Logs a lifecycle event. */
#define SAMPLE_LOG_INFO(category, ...)                                                           \
    SAMPLE_LOG_AT(SushiLog::LogLevel::Info, category, __VA_ARGS__)

/** @brief Logs a recovery or a fallback. */
#define SAMPLE_LOG_WARN(category, ...)                                                           \
    SAMPLE_LOG_AT(SushiLog::LogLevel::Warn, category, __VA_ARGS__)

/** @brief Logs a failed operation. */
#define SAMPLE_LOG_ERROR(category, ...)                                                          \
    SAMPLE_LOG_AT(SushiLog::LogLevel::Error, category, __VA_ARGS__)

/** @brief Logs why the process cannot continue. */
#define SAMPLE_LOG_FATAL(category, ...)                                                          \
    SAMPLE_LOG_AT(SushiLog::LogLevel::Fatal, category, __VA_ARGS__)
