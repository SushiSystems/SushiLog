/**************************************************************************/
/* record_format.hpp                                                      */
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
 * @file record_format.hpp
 * @brief Declares QueuedRecord and make_record, which writes the line of one record.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <SushiLog/log/log_level.hpp>
#include <SushiLog/log/log_record.hpp>

namespace SushiLog::Detail
{
    /**
     * @brief Holds one record on its way to the sink, owning every string the sink's
     *        LogRecord views.
     */
    struct QueuedRecord
    {
        LogLevel level;       /**< The level the record was logged at. */
        std::string project;  /**< The project of its category. */
        std::string module;   /**< The module of its category. */
        std::string file;     /**< The file of the call, without its folders. */
        std::uint32_t line;   /**< The line of the call. */
        std::uint32_t thread; /**< This logger's number for the calling thread. */
        std::int64_t time_ns; /**< The time of the call. */
        std::string message;  /**< The formatted message. */
        std::string text;     /**< The whole line. */

        /** @brief Returns the views a sink receives; valid while this record lives. */
        [[nodiscard]] LogRecord view() const noexcept;
    };

    /** @brief Returns the system clock time now, in nanoseconds since the Unix epoch. */
    [[nodiscard]] std::int64_t now_ns() noexcept;

    /**
     * @brief Returns this logger's number for the calling thread, given on its first call.
     */
    [[nodiscard]] std::uint32_t thread_number() noexcept;

    /**
     * @brief Returns a record of the calling thread with its line written: the local time, the
     *        level, the project and module, the thread, the file and line, the message.
     * @param file The path of the call; the record keeps the name after its last separator.
     */
    [[nodiscard]] QueuedRecord make_record(LogLevel level, std::string_view project,
                                           std::string_view module, std::string_view file,
                                           std::uint32_t line, std::int64_t time_ns,
                                           std::string&& message);
} // namespace SushiLog::Detail
