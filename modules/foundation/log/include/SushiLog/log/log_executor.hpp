/**************************************************************************/
/* log_executor.hpp                                                       */
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
 * @file log_executor.hpp
 * @brief Declares LogExecutor, the port that runs the logger's delivery, and ScopedLogExecutor.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <utility>

#include <SushiLog/log/delegate.hpp>
#include <SushiLog/log/export.hpp>

namespace SushiLog
{
    /** @brief One run of the logger's delivery: it hands every queued record to the sink. */
    using LogTask = Delegate<void(), 32>;

    /**
     * @brief Arranges for a LogTask to run, now or later, on a thread of its own choosing.
     *
     * It is called on the logging thread when a record meets an empty schedule, with no logger
     * lock held, at most once until that task has started. It must not throw. If it does, the
     * logger runs the task on the calling thread.
     */
    using LogExecutor = Delegate<void(LogTask), 32>;

    /**
     * @brief Makes @p executor run the delivery of records, in place of the logger's own thread.
     *
     * Records already queued are scheduled on it. FATAL, set_log_synchronous(true) and
     * shut-down at exit deliver on the calling thread, after every record queued before them.
     * @param executor An empty executor restores the default, the logger's own thread.
     */
    SUSHILOG_API void set_log_executor(LogExecutor executor);

    /** @brief Sets an executor for the life of this object, then restores the default. */
    class ScopedLogExecutor final
    {
        public:
            /** @brief Makes @p executor the executor of the logger until this object ends. */
            explicit ScopedLogExecutor(LogExecutor executor)
            {
                set_log_executor(std::move(executor));
            }

            ScopedLogExecutor(const ScopedLogExecutor&) = delete;
            ScopedLogExecutor& operator=(const ScopedLogExecutor&) = delete;
            ScopedLogExecutor(ScopedLogExecutor&&) = delete;
            ScopedLogExecutor& operator=(ScopedLogExecutor&&) = delete;

            /** @brief Restores the default executor. */
            ~ScopedLogExecutor()
            {
                set_log_executor(LogExecutor());
            }
    };
} // namespace SushiLog
