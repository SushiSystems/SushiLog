/**************************************************************************/
/* log_calls.hpp                                                          */
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
 * @file log_calls.hpp
 * @brief Declares the calls that set the levels, the sink and the delivery mode of the logger.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <SushiLog/log/export.hpp>
#include <SushiLog/log/log_category.hpp>
#include <SushiLog/log/log_level.hpp>
#include <SushiLog/log/log_project.hpp>
#include <SushiLog/log/log_sink.hpp>

#include <string_view>

namespace SushiLog
{
    /**
     * @brief Sets every category of @p project except those set one by one.
     *
     * For a project none of whose categories this logger has seen, it changes nothing and
     * writes one WARN naming the project.
     */
    SUSHILOG_API void set_log_level(LogProject project, LogLevel level) noexcept;

    /**
     * @brief Returns the level of the categories of @p project that no module setting overrides.
     * @return Warn, with one WARN written, for a project this logger has not seen.
     */
    [[nodiscard]] SUSHILOG_API LogLevel log_level(LogProject project) noexcept;

    /**
     * @brief Sets one category, which set_log_level(project, level) then leaves alone.
     *
     * Naming the category makes this logger see it and read its project's variable first.
     */
    SUSHILOG_API void set_log_level(LogCategory category, LogLevel level) noexcept;

    /** @brief Returns the level of one category, reading its project's variable on first use. */
    [[nodiscard]] SUSHILOG_API LogLevel log_level(LogCategory category) noexcept;

    /**
     * @brief Replaces the sink of the process and returns once no call into the old one runs.
     * @param sink An empty sink restores the default, which writes every line to stderr.
     */
    SUSHILOG_API void set_log_sink(LogSink sink);

    /**
     * @brief Chooses whether a record reaches the sink before its log call returns.
     * @param synchronous True first waits for every queued record, then makes each log call
     *        deliver its own record; false hands records to the logger's thread.
     */
    SUSHILOG_API void set_log_synchronous(bool synchronous);

    /**
     * @brief Returns a sink that enters another logger's records into this logger's queue.
     *
     * A forwarded record passes no level gate here. It keeps its time and call site, takes
     * this logger's number for the calling thread and gets its line written here.
     */
    [[nodiscard]] SUSHILOG_API LogSink log_forward_sink();

    /**
     * @brief Writes one record of @p message at the call site @p file and @p line, past both
     *        level gates.
     *
     * For a handler that reports a failure at another call site, such as an assertion, whose
     * record must carry the failing line and must not be dropped by a level.
     */
    SUSHILOG_API void log_at(LogLevel level, LogCategory category, const char* file, int line,
                             std::string_view message) noexcept;
} // namespace SushiLog
