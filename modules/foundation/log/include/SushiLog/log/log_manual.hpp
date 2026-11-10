/**************************************************************************/
/* log_manual.hpp                                                         */
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
 * @file log_manual.hpp
 * @brief Defines ManualLogClock and ManualLogExecutor, the in-memory clock and executor adapters.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>
#include <utility>

#include <SushiLog/log/log_clock.hpp>
#include <SushiLog/log/log_executor.hpp>

namespace SushiLog
{
    /** @brief A clock that moves only when the test says. It outlives the LogClock it hands out. */
    class ManualLogClock final
    {
        public:
            /** @brief Starts at @p start_ns nanoseconds since the Unix epoch. */
            explicit ManualLogClock(std::int64_t start_ns = 0) noexcept
                : now_ns_(start_ns)
            {
            }

            ManualLogClock(const ManualLogClock&) = delete;
            ManualLogClock& operator=(const ManualLogClock&) = delete;
            ManualLogClock(ManualLogClock&&) = delete;
            ManualLogClock& operator=(ManualLogClock&&) = delete;
            ~ManualLogClock() = default;

            /** @brief Returns the time now, in nanoseconds since the Unix epoch. */
            [[nodiscard]] std::int64_t now_ns() const noexcept
            {
                return now_ns_.load(std::memory_order_relaxed);
            }

            /** @brief Moves the time on by @p delta_ns nanoseconds. */
            void advance(std::int64_t delta_ns) noexcept
            {
                now_ns_.fetch_add(delta_ns, std::memory_order_relaxed);
            }

            /** @brief Returns a LogClock that reads this clock; this object must outlive it. */
            [[nodiscard]] LogClock as_log_clock() noexcept
            {
                return LogClock([this]() noexcept
                {
                    return now_ns();
                });
            }

        private:
            /// @brief The time now.
            std::atomic<std::int64_t> now_ns_;
    };

    /** @brief An executor that holds the tasks it is given until the test runs them. */
    class ManualLogExecutor final
    {
        public:
            ManualLogExecutor() = default;
            ManualLogExecutor(const ManualLogExecutor&) = delete;
            ManualLogExecutor& operator=(const ManualLogExecutor&) = delete;
            ManualLogExecutor(ManualLogExecutor&&) = delete;
            ManualLogExecutor& operator=(ManualLogExecutor&&) = delete;
            ~ManualLogExecutor() = default;

            /** @brief Returns a LogExecutor that queues here; this object must outlive it. */
            [[nodiscard]] LogExecutor as_log_executor()
            {
                return LogExecutor([this](LogTask task)
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    tasks_.push_back(std::move(task));
                });
            }

            /** @brief Returns how many tasks wait to run. */
            [[nodiscard]] std::size_t pending() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return tasks_.size();
            }

            /**
             * @brief Runs the waiting tasks on this thread, oldest first, with no lock held.
             * @return How many ran; a task queued while they ran waits for the next call.
             */
            std::size_t run_pending()
            {
                std::deque<LogTask> batch;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    batch.swap(tasks_);
                }
                for (const LogTask& task : batch)
                    task();
                return batch.size();
            }

        private:
            /// @brief Guards tasks_.
            mutable std::mutex mutex_;
            /// @brief The tasks waiting to run, oldest first.
            std::deque<LogTask> tasks_;
    };
} // namespace SushiLog
