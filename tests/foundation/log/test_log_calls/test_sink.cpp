/**************************************************************************/
/* test_sink.cpp                                                          */
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
 * @file test_sink.cpp
 * @brief Tests the default stderr sink, replacing the sink, a sink that throws and both modes.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <SushiLog/log/log_capture.hpp>
#include "delivery_mode_guard.hpp"
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_SINK_PROJECT{"sinktest", "SINKTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_OUTPUT_LOG_CATEGORY =
        SushiLog::make_log_category(K_SINK_PROJECT, "output");
} // namespace

TEST(Medium_LogSink, TheDefaultSinkWritesEachLineToStderr)
{
    const SushiLogTests::SinkGuard sink{SushiLog::LogSink()};
    const SushiLogTests::DeliveryModeGuard mode(true);

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "to stderr");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_NE(err.find("[WARN] [sinktest.output]"), std::string::npos);
    EXPECT_NE(err.find("to stderr\n"), std::string::npos);
}

TEST(Medium_LogSink, AnEmptySinkRestoresStderr)
{
    SushiLog::LogRecorder recording;
    const SushiLogTests::SinkGuard guard{SushiLog::LogSink()};
    const SushiLogTests::DeliveryModeGuard mode(true);
    SushiLog::set_log_sink(recording.sink());
    SushiLog::set_log_sink(SushiLog::LogSink());

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "restored");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_EQ(recording.count_of("restored"), 0u);
    EXPECT_NE(err.find("restored"), std::string::npos);
}

TEST(Medium_LogSink, ExchangeHandsBackTheSinkItReplaced)
{
    SushiLog::LogRecorder first;
    SushiLog::LogRecorder second;
    const SushiLogTests::SinkGuard guard(first.sink());
    const SushiLogTests::DeliveryModeGuard mode(true);
    SushiLog::LogSink swapped = second.sink();

    const bool exchanged = SushiLog::exchange_log_sink(swapped);
    swapped(SushiLog::LogRecord{});

    EXPECT_TRUE(exchanged);
    EXPECT_EQ(first.records().size(), 1u);
    EXPECT_TRUE(second.records().empty());
}

TEST(Medium_LogSink, ASinkThatThrowsLosesNothing)
{
    const SushiLogTests::SinkGuard sink(SushiLog::LogSink([](const SushiLog::LogRecord&)
    {
        throw std::runtime_error("disk full");
    }));
    const SushiLogTests::DeliveryModeGuard mode(true);

    testing::internal::CaptureStderr();
    SAMPLE_LOG_ERROR(K_OUTPUT_LOG_CATEGORY, "kept anyway");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_NE(err.find("disk full"), std::string::npos);
    EXPECT_NE(err.find("kept anyway"), std::string::npos);
}

TEST(Medium_LogSink, AsynchronousDeliveryRunsOnTheLoggerThread)
{
    std::thread::id delivered_on;
    const SushiLogTests::SinkGuard sink(
        SushiLog::LogSink([&delivered_on](const SushiLog::LogRecord&)
        {
            delivered_on = std::this_thread::get_id();
        }));
    const SushiLogTests::DeliveryModeGuard mode(false);

    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "queued");
    SushiLog::set_log_synchronous(true);

    EXPECT_NE(delivered_on, std::thread::id());
    EXPECT_NE(delivered_on, std::this_thread::get_id());
}

TEST(Medium_LogSink, EnteringSynchronousModeWaitsForTheQueueInOrder)
{
    SushiLog::LogRecorder recording;
    const SushiLogTests::SinkGuard sink(recording.sink());
    const SushiLogTests::DeliveryModeGuard mode(false);

    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "record 0");
    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "record 1");
    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "record 2");
    SushiLog::set_log_synchronous(true);

    const std::vector<SushiLog::CapturedRecord> lines = recording.records();
    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0].message, "record 0");
    EXPECT_EQ(lines[1].message, "record 1");
    EXPECT_EQ(lines[2].message, "record 2");
}

TEST(Medium_LogSink, SynchronousModeDeliversOnTheCallingThread)
{
    std::thread::id delivered_on;
    const SushiLogTests::SinkGuard sink(
        SushiLog::LogSink([&delivered_on](const SushiLog::LogRecord&)
        {
            delivered_on = std::this_thread::get_id();
        }));
    const SushiLogTests::DeliveryModeGuard mode(true);

    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "here");

    EXPECT_EQ(delivered_on, std::this_thread::get_id());
}
