/**************************************************************************/
/* log_clock.hpp                                                          */
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
 * @file log_clock.hpp
 * @brief Declares LogClock, the clock a record takes its time from, and ScopedLogClock.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <cstdint>
#include <utility>

#include <SushiLog/log/delegate.hpp>
#include <SushiLog/log/export.hpp>

namespace SushiLog
{
    /**
     * @brief Returns the time of a record, in nanoseconds since the Unix epoch. It is called
     *        on the logging thread, must not throw, and does not log.
     */
    using LogClock = Delegate<std::int64_t(), 32>;

    /** @brief Returns the system clock now, in nanoseconds since the Unix epoch. */
    [[nodiscard]] SUSHILOG_API std::int64_t system_clock_ns() noexcept;

    /**
     * @brief Makes @p clock the clock every record takes its time from and returns the one it
     *        replaced, empty for the default.
     * @param clock An empty clock restores the default, system_clock_ns().
     */
    [[nodiscard]] SUSHILOG_API LogClock exchange_log_clock(LogClock clock);

    /** @brief Sets a clock for the life of this object, then puts back the one it found. */
    class ScopedLogClock final
    {
        public:
            /** @brief Makes @p clock the clock of every record until this object ends. */
            explicit ScopedLogClock(LogClock clock)
                : previous_(exchange_log_clock(std::move(clock)))
            {
            }

            ScopedLogClock(const ScopedLogClock&) = delete;
            ScopedLogClock& operator=(const ScopedLogClock&) = delete;
            ScopedLogClock(ScopedLogClock&&) = delete;
            ScopedLogClock& operator=(ScopedLogClock&&) = delete;

            /** @brief Puts back the clock found on entry. */
            ~ScopedLogClock()
            {
                static_cast<void>(exchange_log_clock(std::move(previous_)));
            }

        private:
            /// @brief The clock found on entry.
            LogClock previous_;
    };
} // namespace SushiLog
