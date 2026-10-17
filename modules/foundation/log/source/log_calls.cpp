/**************************************************************************/
/* log_calls.cpp                                                          */
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
 * @file log_calls.cpp
 * @brief Defines the calls that set the levels, the sink and the delivery mode of the logger.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <exception>
#include <string>
#include <utility>

#include <SushiLog/log/log_front.hpp>

#include "forward_sink.hpp"
#include "level_name.hpp"
#include "logger.hpp"
#include "stderr_line.hpp"

namespace SushiLog
{
    namespace
    {
        /** @brief Writes on stderr that @p call failed with @p error and changed nothing. */
        void report_failed_call(std::string_view call, const std::exception& error) noexcept
        {
            Detail::write_stderr_line("[SushiLog] " + std::string(call) + " failed (" +
                                      error.what() + "); nothing changed.");
        }
    } // namespace

    void set_log_level(LogProject project, LogLevel level) noexcept
    {
        try
        {
            Detail::logger().levels.set_project(project, level);
        }
        catch (const std::exception& error)
        {
            report_failed_call("set_log_level(project)", error);
        }
    }

    LogLevel log_level(LogProject project) noexcept
    {
        try
        {
            return Detail::logger().levels.project_level(project);
        }
        catch (const std::exception& error)
        {
            report_failed_call("log_level(project)", error);
        }
        return LogLevel::Warn;
    }

    void set_log_level(LogCategory category, LogLevel level) noexcept
    {
        try
        {
            Detail::logger().levels.set_category(category, level);
        }
        catch (const std::exception& error)
        {
            report_failed_call("set_log_level(category)", error);
        }
    }

    LogLevel log_level(LogCategory category) noexcept
    {
        return static_cast<LogLevel>(
            Detail::log_level_slot(category).load(std::memory_order_relaxed));
    }

    void set_log_sink(LogSink sink)
    {
        static_cast<void>(Detail::logger().sink.replace(std::move(sink)));
    }

    void set_log_synchronous(bool synchronous)
    {
        Detail::logger().writer.set_synchronous(synchronous);
    }

    void log_at(LogLevel level, LogCategory category, const char* file, int line,
                std::string_view message) noexcept
    {
        try
        {
            Detail::emit_log_record(level, category, file, line, std::string(message));
        }
        catch (const std::exception& error)
        {
            report_failed_call("log_at", error);
        }
    }

    LogSink log_forward_sink()
    {
        return LogSink([](const LogRecord& record)
        {
            Detail::logger().writer.write(Detail::forwarded_record(record));
        });
    }
} // namespace SushiLog
