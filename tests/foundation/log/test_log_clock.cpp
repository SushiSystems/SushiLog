/**************************************************************************/
/* test_log_clock.cpp                                                     */
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
 * @file test_log_clock.cpp
 * @brief Tests the clock a record takes its time from.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_clock.hpp>

#include <cstdint>

#include <gtest/gtest.h>

#include <SushiLog/log/log_capture.hpp>
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_CLOCK_PROJECT{"clocktest", "CLOCKTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_CLOCK_LOG_CATEGORY =
        SushiLog::make_log_category(K_CLOCK_PROJECT, "clock");

    /// @brief The time the set clock gives, 2026-01-01 00:00:00 UTC in nanoseconds.
    constexpr std::int64_t K_SET_TIME_NS = 1767225600000000000;

    /** @brief Returns K_SET_TIME_NS. */
    std::int64_t set_time()
    {
        return K_SET_TIME_NS;
    }
} // namespace

TEST(Small_LogClock, ARecordTakesTheTimeOfTheSetClock)
{
    const SushiLog::LogCapture recording;
    const SushiLog::ScopedLogClock clock{SushiLog::LogClock(&set_time)};

    SAMPLE_LOG_WARN(K_CLOCK_LOG_CATEGORY, "timed");

    ASSERT_EQ(recording.records().size(), 1u);
    EXPECT_EQ(recording.records()[0].time_ns, K_SET_TIME_NS);
}

TEST(Small_LogClock, ExchangeHandsBackTheClockItReplaced)
{
    const SushiLog::ScopedLogClock clock{SushiLog::LogClock(&set_time)};

    SushiLog::LogClock previous = SushiLog::exchange_log_clock(SushiLog::LogClock());

    ASSERT_TRUE(static_cast<bool>(previous));
    EXPECT_EQ(previous(), K_SET_TIME_NS);
    static_cast<void>(SushiLog::exchange_log_clock(std::move(previous)));
}

TEST(Medium_LogClock, TheDefaultClockIsTheSystemClock)
{
    const SushiLog::LogCapture recording;
    const std::int64_t before = SushiLog::system_clock_ns();

    SAMPLE_LOG_WARN(K_CLOCK_LOG_CATEGORY, "timed");

    const std::int64_t after = SushiLog::system_clock_ns();
    ASSERT_EQ(recording.records().size(), 1u);
    EXPECT_GE(recording.records()[0].time_ns, before);
    EXPECT_LE(recording.records()[0].time_ns, after);
}
