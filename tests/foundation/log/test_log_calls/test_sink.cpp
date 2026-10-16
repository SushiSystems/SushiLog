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

#include "recording_sink.hpp"
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_SINK_PROJECT{"sinktest", "SINKTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_OUTPUT_LOG_CATEGORY =
        SushiLog::make_log_category(K_SINK_PROJECT, "output");
} // namespace

TEST(Medium_LogSink, TheDefaultSinkWritesEachLineToStderr)
{
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink());

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "to stderr");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_NE(err.find("[WARN] [sinktest.output]"), std::string::npos);
    EXPECT_NE(err.find("to stderr\n"), std::string::npos);
}

TEST(Medium_LogSink, AnEmptySinkRestoresStderr)
{
    SushiLog::set_log_synchronous(true);
    SushiLogTests::RecordingSink recording;
    SushiLog::set_log_sink(recording.sink());
    SushiLog::set_log_sink(SushiLog::LogSink());

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "restored");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_EQ(recording.count_of("restored"), 0u);
    EXPECT_NE(err.find("restored"), std::string::npos);
}

TEST(Medium_LogSink, ASinkThatThrowsLosesNothing)
{
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink([](const SushiLog::LogRecord&)
    {
        throw std::runtime_error("disk full");
    }));

    testing::internal::CaptureStderr();
    SAMPLE_LOG_ERROR(K_OUTPUT_LOG_CATEGORY, "kept anyway");
    const std::string err = testing::internal::GetCapturedStderr();
    SushiLog::set_log_sink(SushiLog::LogSink());

    EXPECT_NE(err.find("disk full"), std::string::npos);
    EXPECT_NE(err.find("kept anyway"), std::string::npos);
}

TEST(Medium_LogSink, AsynchronousDeliveryRunsOnTheLoggerThread)
{
    std::thread::id delivered_on;
    SushiLog::set_log_sink(SushiLog::LogSink([&delivered_on](const SushiLog::LogRecord&)
    {
        delivered_on = std::this_thread::get_id();
    }));
    SushiLog::set_log_synchronous(false);

    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "queued");
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink());

    EXPECT_NE(delivered_on, std::thread::id());
    EXPECT_NE(delivered_on, std::this_thread::get_id());
}

TEST(Medium_LogSink, EnteringSynchronousModeWaitsForTheQueueInOrder)
{
    SushiLogTests::RecordingSink recording;
    SushiLog::set_log_sink(recording.sink());
    SushiLog::set_log_synchronous(false);

    for (int i = 0; i < 100; ++i)
        SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "record {}", i);
    SushiLog::set_log_synchronous(true);

    const std::vector<SushiLogTests::RecordedLine> lines = recording.lines();
    SushiLog::set_log_sink(SushiLog::LogSink());
    ASSERT_EQ(lines.size(), 100u);
    for (int i = 0; i < 100; ++i)
        EXPECT_EQ(lines[static_cast<std::size_t>(i)].message, "record " + std::to_string(i));
}

TEST(Medium_LogSink, SynchronousModeDeliversOnTheCallingThread)
{
    std::thread::id delivered_on;
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink([&delivered_on](const SushiLog::LogRecord&)
    {
        delivered_on = std::this_thread::get_id();
    }));

    SAMPLE_LOG_WARN(K_OUTPUT_LOG_CATEGORY, "here");
    SushiLog::set_log_sink(SushiLog::LogSink());

    EXPECT_EQ(delivered_on, std::this_thread::get_id());
}
