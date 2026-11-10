/**************************************************************************/
/* test_log_manual.cpp                                                    */
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
 * @file test_log_manual.cpp
 * @brief Tests the manual clock and the manual executor, the adapters a test drives by hand.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_manual.hpp>

#include <gtest/gtest.h>

#include <SushiLog/log/log_capture.hpp>
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_MANUAL_PROJECT{"manualtest", "MANUALTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_MANUAL_LOG_CATEGORY =
        SushiLog::make_log_category(K_MANUAL_PROJECT, "manual");
} // namespace

TEST(Small_LogManual, TheManualClockGivesTheTimeItWasSetTo)
{
    SushiLog::ManualLogClock manual(1000);
    const SushiLog::LogCapture recording;
    const SushiLog::ScopedLogClock clock{manual.as_log_clock()};

    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "first");
    manual.advance(500);
    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "second");

    ASSERT_EQ(recording.records().size(), 2u);
    EXPECT_EQ(recording.records()[0].time_ns, 1000);
    EXPECT_EQ(recording.records()[1].time_ns, 1500);
}

TEST(Small_LogManual, TheManualExecutorHoldsATaskUntilItIsRun)
{
    SushiLog::ManualLogExecutor manual;
    int runs = 0;
    const SushiLog::LogExecutor executor = manual.as_log_executor();

    executor(SushiLog::LogTask([&runs]()
    {
        ++runs;
    }));

    EXPECT_EQ(runs, 0);
    EXPECT_EQ(manual.pending(), 1u);
    EXPECT_EQ(manual.run_pending(), 1u);
    EXPECT_EQ(runs, 1);
    EXPECT_EQ(manual.pending(), 0u);
}
