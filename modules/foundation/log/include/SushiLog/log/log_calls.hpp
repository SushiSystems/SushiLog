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

#include <cstddef>
#include <string_view>

namespace SushiLog
{
    /**
     * @brief Sets every category of @p project except those set one by one.
     *
     * For a project none of whose categories this logger has seen yet, the level is kept and
     * applies to each category of it that registers later. Nothing is written.
     */
    SUSHILOG_API void set_log_level(LogProject project, LogLevel level) noexcept;

    /**
     * @brief Returns the level of the categories of @p project that no module setting overrides.
     * @return The project's level; Warn, with nothing written, for a project this logger has
     *         not seen.
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
     * @brief Replaces the sink of the process with @p sink, once no call into the old one runs,
     *        and hands the old one back in @p sink.
     * @return False, changing nothing on either side, when called from inside a sink.
     */
    [[nodiscard]] SUSHILOG_API bool exchange_log_sink(LogSink& sink);

    /**
     * @brief Chooses whether a record reaches the sink before its log call returns.
     * @param synchronous True first waits for every queued record, then makes each log call
     *        deliver its own record; false hands records to the logger's thread.
     */
    SUSHILOG_API void set_log_synchronous(bool synchronous);

    /** @brief Returns whether a record reaches the sink before its log call returns. */
    [[nodiscard]] SUSHILOG_API bool log_synchronous();

    /**
     * @brief Holds how many records wait for the logger's thread before one is dropped; the
     *        next record delivered after a drop is preceded by a WARN that counts them.
     */
    inline constexpr std::size_t K_LOG_QUEUE_CAPACITY = 4096;

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
