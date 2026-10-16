/**************************************************************************/
/* record_format.cpp                                                      */
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
 * @file record_format.cpp
 * @brief Defines QueuedRecord and make_record, which writes the line of one record.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "record_format.hpp"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <utility>

#include "level_name.hpp"

namespace SushiLog::Detail
{
    namespace
    {
        /** @brief Holds the nanoseconds of one second. */
        constexpr std::int64_t K_NS_PER_SECOND = 1000000000;

        /** @brief Holds the nanoseconds of one millisecond. */
        constexpr std::int64_t K_NS_PER_MILLISECOND = 1000000;

        /** @brief Returns the part of @p path after its last separator. */
        std::string_view file_name(std::string_view path) noexcept
        {
            const std::size_t slash = path.find_last_of("/\\");
            return slash == std::string_view::npos ? path : path.substr(slash + 1);
        }

        /** @brief Converts @p time to local calendar time without shared state. */
        std::tm local_time(std::time_t time) noexcept
        {
            std::tm result{};
            #ifdef _WIN32
                localtime_s(&result, &time);
            #else
                localtime_r(&time, &result);
            #endif
            return result;
        }

        /** @brief Appends the local time of day of @p time_ns to @p out as HH:MM:SS.mmm. */
        void append_timestamp(std::string& out, std::int64_t time_ns)
        {
            std::int64_t seconds = time_ns / K_NS_PER_SECOND;
            std::int64_t rest = time_ns % K_NS_PER_SECOND;
            if (rest < 0)
            {
                --seconds;
                rest += K_NS_PER_SECOND;
            }
            const std::tm calendar = local_time(static_cast<std::time_t>(seconds));

            char buffer[16];
            std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d", calendar.tm_hour,
                          calendar.tm_min, calendar.tm_sec,
                          static_cast<int>(rest / K_NS_PER_MILLISECOND));
            out += buffer;
        }
    } // namespace

    LogRecord QueuedRecord::view() const noexcept
    {
        return LogRecord{level, project, module, file, line, thread, time_ns, message, text};
    }

    std::int64_t now_ns() noexcept
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    std::uint32_t thread_number() noexcept
    {
        static std::atomic<std::uint32_t> next{1};
        thread_local const std::uint32_t number = next.fetch_add(1, std::memory_order_relaxed);
        return number;
    }

    QueuedRecord make_record(LogLevel level, std::string_view project, std::string_view module,
                             std::string_view file, std::uint32_t line, std::int64_t time_ns,
                             std::string&& message)
    {
        QueuedRecord record{level,
                            std::string(project),
                            std::string(module),
                            std::string(file_name(file)),
                            line,
                            thread_number(),
                            time_ns,
                            std::move(message),
                            {}};

        std::string& text = record.text;
        text.reserve(record.message.size() + record.project.size() + record.module.size() +
                     record.file.size() + 48);
        text += '[';
        append_timestamp(text, time_ns);
        text += "] [";
        text += level_label(level);
        text += "] [";
        text += record.project;
        text += '.';
        text += record.module;
        text += "] [t";
        text += std::to_string(record.thread);
        text += "] [";
        text += record.file;
        text += ':';
        text += std::to_string(line);
        text += "] ";
        const std::size_t message_start = text.size();
        text += record.message;
        // A record is one line, so a line break inside the message becomes a space.
        for (std::size_t i = message_start; i < text.size(); ++i)
        {
            if (text[i] == '\n' || text[i] == '\r')
                text[i] = ' ';
        }
        return record;
    }
} // namespace SushiLog::Detail
