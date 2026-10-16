/**************************************************************************/
/* log_c_sink.cpp                                                         */
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
 * @file log_c_sink.cpp
 * @brief Defines the C interface and the size rule of SushiLogRecord.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include <SushiLog/log/log_c_sink.h>

#include <cstdint>
#include <cstring>
#include <exception>
#include <string>
#include <string_view>
#include <utility>

#include <SushiLog/log/log_calls.hpp>
#include <SushiLog/log/log_record.hpp>

#include "forward_sink.hpp"
#include "logger.hpp"
#include "stderr_line.hpp"

namespace
{
    /** @brief Returns @p record in the C layout of log_c_sink.h, its size this version's. */
    SushiLogRecord to_c_record(const SushiLog::LogRecord& record) noexcept
    {
        SushiLogRecord out{};
        out.size = static_cast<std::uint32_t>(sizeof(SushiLogRecord));
        out.level = static_cast<std::uint8_t>(record.level);
        out.project = record.project.data();
        out.project_size = record.project.size();
        out.module = record.module.data();
        out.module_size = record.module.size();
        out.file = record.file.data();
        out.file_size = record.file.size();
        out.line = record.line;
        out.thread = record.thread;
        out.time_ns = record.time_ns;
        out.message = record.message.data();
        out.message_size = record.message.size();
        out.text = record.text.data();
        out.text_size = record.text.size();
        return out;
    }

    /**
     * @brief Returns the fields of @p record its @c size covers; a field past it reads as
     *        zero or empty.
     */
    SushiLogRecord covered_fields(const SushiLogRecord& record) noexcept
    {
        SushiLogRecord out{};
        std::size_t size = record.size;
        if (size > sizeof(SushiLogRecord))
            size = sizeof(SushiLogRecord);
        std::memcpy(&out, &record, size);
        return out;
    }

    /** @brief Returns the view of @p size characters at @p data, empty for a null pointer. */
    std::string_view view_of(const char* data, std::size_t size) noexcept
    {
        return data == nullptr ? std::string_view() : std::string_view(data, size);
    }

    /** @brief Returns the level a C record carries, Fatal for a number past it. */
    SushiLog::LogLevel level_of(std::uint8_t level) noexcept
    {
        if (level > static_cast<std::uint8_t>(SushiLog::LogLevel::Fatal))
            return SushiLog::LogLevel::Fatal;
        return static_cast<SushiLog::LogLevel>(level);
    }

    /** @brief Writes on stderr that @p call failed with @p error. */
    void report_failed_call(std::string_view call, const std::exception& error) noexcept
    {
        SushiLog::Detail::write_stderr_line("[SushiLog] " + std::string(call) + " failed (" +
                                            error.what() + "); nothing changed.");
    }
} // namespace

extern "C" void sl_set_log_sink(SushiLogSinkFunction function, void* user)
{
    try
    {
        if (function == nullptr)
        {
            SushiLog::set_log_sink(SushiLog::LogSink());
            return;
        }
        SushiLog::LogSink sink([function, user](const SushiLog::LogRecord& record)
        {
            const SushiLogRecord c_record = to_c_record(record);
            function(&c_record, user);
        });
        SushiLog::set_log_sink(std::move(sink));
    }
    catch (const std::exception& error)
    {
        report_failed_call("sl_set_log_sink", error);
    }
}

extern "C" void sl_set_log_synchronous(int synchronous)
{
    try
    {
        SushiLog::set_log_synchronous(synchronous != 0);
    }
    catch (const std::exception& error)
    {
        report_failed_call("sl_set_log_synchronous", error);
    }
}

extern "C" void sl_forward_record(const SushiLogRecord* record, void* user)
{
    static_cast<void>(user);
    if (record == nullptr)
        return;
    try
    {
        const SushiLogRecord fields = covered_fields(*record);
        const SushiLog::LogRecord view{level_of(fields.level),
                                       view_of(fields.project, fields.project_size),
                                       view_of(fields.module, fields.module_size),
                                       view_of(fields.file, fields.file_size),
                                       fields.line,
                                       fields.thread,
                                       fields.time_ns,
                                       view_of(fields.message, fields.message_size),
                                       view_of(fields.text, fields.text_size)};
        SushiLog::Detail::logger().writer.write(SushiLog::Detail::forwarded_record(view));
    }
    catch (const std::exception& error)
    {
        report_failed_call("sl_forward_record", error);
    }
}
