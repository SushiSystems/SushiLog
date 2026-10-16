/**************************************************************************/
/* recording_sink.hpp                                                     */
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
 * @file recording_sink.hpp
 * @brief Declares RecordingSink, a sink the tests of the log module read records back from.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include <SushiLog/log/log_calls.hpp>
#include <SushiLog/log/log_level.hpp>
#include <SushiLog/log/log_record.hpp>
#include <SushiLog/log/log_sink.hpp>

namespace SushiLogTests
{
    /** @brief Holds one delivered record, owning every field the sink's views showed. */
    struct RecordedLine
    {
        SushiLog::LogLevel level; /**< The record's level. */
        std::string project;      /**< Its project. */
        std::string module;       /**< Its module. */
        std::string file;         /**< Its file. */
        std::uint32_t line;       /**< Its line. */
        std::uint32_t thread;     /**< The logger's number of the calling thread. */
        std::int64_t time_ns;     /**< Its time. */
        std::string message;      /**< Its message. */
        std::string text;         /**< Its whole line. */
    };

    /**
     * @brief Keeps every record its sink receives, from any thread, in delivery order.
     */
    class RecordingSink final
    {
        public:
            RecordingSink() = default;
            RecordingSink(const RecordingSink&) = delete;
            RecordingSink& operator=(const RecordingSink&) = delete;
            RecordingSink(RecordingSink&&) = delete;
            RecordingSink& operator=(RecordingSink&&) = delete;
            ~RecordingSink() = default;

            /** @brief Returns a sink that records into this object, which must outlive it. */
            [[nodiscard]] SushiLog::LogSink sink()
            {
                return SushiLog::LogSink([this](const SushiLog::LogRecord& record)
                {
                    keep(record);
                });
            }

            /** @brief Returns a copy of every record kept so far. */
            [[nodiscard]] std::vector<RecordedLine> lines() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return lines_;
            }

            /** @brief Returns the records kept whose message is @p message. */
            [[nodiscard]] std::size_t count_of(const std::string& message) const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                std::size_t count = 0;
                for (const RecordedLine& line : lines_)
                {
                    if (line.message == message)
                        ++count;
                }
                return count;
            }

        private:
            /** @brief Copies @p record into lines_. */
            void keep(const SushiLog::LogRecord& record)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                lines_.push_back(RecordedLine{record.level, std::string(record.project),
                                              std::string(record.module),
                                              std::string(record.file), record.line,
                                              record.thread, record.time_ns,
                                              std::string(record.message),
                                              std::string(record.text)});
            }

            /// @brief Guards lines_.
            mutable std::mutex mutex_;
            /// @brief The records in delivery order.
            std::vector<RecordedLine> lines_;
    };

    /**
     * @brief Sets synchronous delivery into a RecordingSink for one scope, then restores
     *        stderr and asynchronous delivery.
     */
    class SynchronousRecording final
    {
        public:
            SynchronousRecording()
            {
                SushiLog::set_log_synchronous(true);
                SushiLog::set_log_sink(sink_.sink());
            }

            SynchronousRecording(const SynchronousRecording&) = delete;
            SynchronousRecording& operator=(const SynchronousRecording&) = delete;
            SynchronousRecording(SynchronousRecording&&) = delete;
            SynchronousRecording& operator=(SynchronousRecording&&) = delete;

            ~SynchronousRecording()
            {
                SushiLog::set_log_sink(SushiLog::LogSink());
            }

            /** @brief Returns the sink records reach while this object lives. */
            [[nodiscard]] const RecordingSink& sink() const noexcept
            {
                return sink_;
            }

        private:
            /// @brief The sink set for the scope.
            RecordingSink sink_;
    };
} // namespace SushiLogTests
