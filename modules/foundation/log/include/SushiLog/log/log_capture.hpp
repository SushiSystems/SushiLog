/**************************************************************************/
/* log_capture.hpp                                                        */
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
 * @file log_capture.hpp
 * @brief Declares LogRecorder, the in-memory sink, and LogCapture, which sets it for one scope.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <SushiLog/log/log_calls.hpp>
#include <SushiLog/log/log_level.hpp>
#include <SushiLog/log/log_record.hpp>
#include <SushiLog/log/log_sink.hpp>

namespace SushiLog
{
    /** @brief Holds one delivered record, owning every field the sink's views showed. */
    struct CapturedRecord
    {
        LogLevel level;       /**< The record's level. */
        std::string project;  /**< Its project. */
        std::string module;   /**< Its module. */
        std::string file;     /**< Its file. */
        std::uint32_t line;   /**< Its line. */
        std::uint32_t thread; /**< The logger's number of the calling thread. */
        std::int64_t time_ns; /**< Its time. */
        std::string message;  /**< Its message. */
        std::string text;     /**< Its whole line. */
    };

    /**
     * @brief Keeps in memory every record its sink receives, from any thread, in delivery
     *        order.
     */
    class LogRecorder final
    {
        public:
            LogRecorder() = default;
            LogRecorder(const LogRecorder&) = delete;
            LogRecorder& operator=(const LogRecorder&) = delete;
            LogRecorder(LogRecorder&&) = delete;
            LogRecorder& operator=(LogRecorder&&) = delete;
            ~LogRecorder() = default;

            /** @brief Returns a sink that records into this object, which must outlive it. */
            [[nodiscard]] LogSink sink()
            {
                return LogSink([this](const LogRecord& record)
                {
                    keep(record);
                });
            }

            /** @brief Returns a copy of every record kept so far, in delivery order. */
            [[nodiscard]] std::vector<CapturedRecord> records() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return records_;
            }

            /** @brief Returns how many records kept so far carry @p message. */
            [[nodiscard]] std::size_t count_of(const std::string& message) const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                std::size_t count = 0;
                for (const CapturedRecord& record : records_)
                {
                    if (record.message == message)
                        ++count;
                }
                return count;
            }

        private:
            /** @brief Copies @p record into records_. */
            void keep(const LogRecord& record)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                records_.push_back(CapturedRecord{record.level, std::string(record.project),
                                                  std::string(record.module),
                                                  std::string(record.file), record.line,
                                                  record.thread, record.time_ns,
                                                  std::string(record.message),
                                                  std::string(record.text)});
            }

            /// @brief Guards records_.
            mutable std::mutex mutex_;
            /// @brief The records in delivery order.
            std::vector<CapturedRecord> records_;
    };

    /**
     * @brief Sends every record of the process to an in-memory LogRecorder, delivered before
     *        its log call returns, for the life of this object.
     *
     * On exit it puts back the sink and the delivery mode it found. Two captures nest; the
     * inner one sees the records of its own scope. Made from inside a sink, it captures
     * nothing and restores nothing, and the logger says so on stderr.
     */
    class LogCapture final
    {
        public:
            LogCapture()
                : was_synchronous_(log_synchronous())
            {
                set_log_synchronous(true);
                previous_ = recorder_.sink();
                installed_ = exchange_log_sink(previous_);
            }

            LogCapture(const LogCapture&) = delete;
            LogCapture& operator=(const LogCapture&) = delete;
            LogCapture(LogCapture&&) = delete;
            LogCapture& operator=(LogCapture&&) = delete;

            ~LogCapture()
            {
                if (installed_)
                    static_cast<void>(exchange_log_sink(previous_));
                set_log_synchronous(was_synchronous_);
            }

            /** @brief Returns a copy of every record captured so far, in delivery order. */
            [[nodiscard]] std::vector<CapturedRecord> records() const
            {
                return recorder_.records();
            }

            /** @brief Returns how many records captured so far carry @p message. */
            [[nodiscard]] std::size_t count_of(const std::string& message) const
            {
                return recorder_.count_of(message);
            }

        private:
            /// @brief The delivery mode found on entry.
            bool was_synchronous_;
            /// @brief Whether the recorder's sink went in, so the exit has one to swap out.
            bool installed_ = false;
            /// @brief The records of this scope.
            LogRecorder recorder_;
            /// @brief The sink found on entry, put back on exit.
            LogSink previous_;
    };
} // namespace SushiLog
