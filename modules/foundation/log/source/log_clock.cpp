/**************************************************************************/
/* log_clock.cpp                                                          */
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
 * @file log_clock.cpp
 * @brief Defines the clock records take their time from and its exchange.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include <SushiLog/log/log_clock.hpp>

#include <atomic>
#include <chrono>
#include <mutex>

#include "log_clock_holder.hpp"

namespace SushiLog
{
    namespace
    {
        /** @brief Holds the clock set by exchange_log_clock, and whether one is set. */
        struct ClockHolder
        {
            std::mutex mutex;               /**< Guards clock. */
            LogClock clock;                 /**< The clock set; empty for the default. */
            std::atomic<bool> set{false};   /**< Whether clock is not empty. */
        };

        /** @brief Returns the one holder, made on first use and never destroyed. */
        ClockHolder& holder()
        {
            static ClockHolder* const held = new ClockHolder();
            return *held;
        }
    } // namespace

    std::int64_t system_clock_ns() noexcept
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    LogClock exchange_log_clock(LogClock clock)
    {
        ClockHolder& held = holder();
        std::lock_guard<std::mutex> lock(held.mutex);
        std::swap(held.clock, clock);
        held.set.store(static_cast<bool>(held.clock), std::memory_order_release);
        return clock;
    }

    namespace Detail
    {
        std::int64_t record_time_ns() noexcept
        {
            ClockHolder& held = holder();
            if (!held.set.load(std::memory_order_acquire))
                return system_clock_ns();
            std::lock_guard<std::mutex> lock(held.mutex);
            return held.clock ? held.clock() : system_clock_ns();
        }
    } // namespace Detail
} // namespace SushiLog
