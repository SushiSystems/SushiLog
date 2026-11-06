/**************************************************************************/
/* test_reentry.cpp                                                       */
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
 * @file test_reentry.cpp
 * @brief Tests that a sink that logs, sets the sink or sets the mode neither deadlocks nor loses.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <string>

#include <gtest/gtest.h>

#include <SushiLog/log/log_capture.hpp>
#include "delivery_mode_guard.hpp"
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_REENTRY_PROJECT{"reentrytest", "REENTRYTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_LOOP_LOG_CATEGORY =
        SushiLog::make_log_category(K_REENTRY_PROJECT, "loop");
} // namespace

TEST(Medium_LogReentry, ARecordLoggedFromInsideTheSinkGoesToStderr)
{
    int calls = 0;
    const SushiLogTests::SinkGuard sink(SushiLog::LogSink([&calls](const SushiLog::LogRecord&)
    {
        ++calls;
        SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "from inside");
    }));
    const SushiLogTests::DeliveryModeGuard mode(true);

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "outside");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_EQ(calls, 1);
    EXPECT_NE(err.find("a sink logged from inside its own call"), std::string::npos);
    EXPECT_NE(err.find("from inside"), std::string::npos);
}

TEST(Medium_LogReentry, ASinkThatSetsTheSinkChangesNothing)
{
    SushiLog::LogRecorder other;
    int calls = 0;
    const SushiLogTests::SinkGuard sink(
        SushiLog::LogSink([&calls, &other](const SushiLog::LogRecord&)
        {
            ++calls;
            SushiLog::set_log_sink(other.sink());
        }));
    const SushiLogTests::DeliveryModeGuard mode(true);

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "one");
    SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "two");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_EQ(calls, 2);
    EXPECT_TRUE(other.records().empty());
    EXPECT_NE(err.find("a sink set the sink from inside its own call"), std::string::npos);
}

TEST(Medium_LogReentry, ASinkThatSetsTheModeChangesNothing)
{
    const SushiLogTests::SinkGuard sink(SushiLog::LogSink([](const SushiLog::LogRecord&)
    {
        SushiLog::set_log_synchronous(false);
    }));
    const SushiLogTests::DeliveryModeGuard mode(true);

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "mode");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_NE(err.find("a sink set the delivery mode from inside its own call"),
              std::string::npos);
    EXPECT_TRUE(SushiLog::log_synchronous());
}
