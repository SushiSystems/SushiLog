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
 * @file test_reentry.cpp
 * @brief Tests that a sink that logs, sets the sink or sets the mode neither deadlocks nor loses.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <string>

#include <gtest/gtest.h>

#include "recording_sink.hpp"
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
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink([&calls](const SushiLog::LogRecord&)
    {
        ++calls;
        SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "from inside");
    }));

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "outside");
    const std::string err = testing::internal::GetCapturedStderr();
    SushiLog::set_log_sink(SushiLog::LogSink());

    EXPECT_EQ(calls, 1);
    EXPECT_NE(err.find("a sink logged from inside its own call"), std::string::npos);
    EXPECT_NE(err.find("from inside"), std::string::npos);
}

TEST(Medium_LogReentry, ASinkThatSetsTheSinkChangesNothing)
{
    SushiLogTests::RecordingSink other;
    int calls = 0;
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink([&calls, &other](const SushiLog::LogRecord&)
    {
        ++calls;
        SushiLog::set_log_sink(other.sink());
    }));

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "one");
    SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "two");
    const std::string err = testing::internal::GetCapturedStderr();
    SushiLog::set_log_sink(SushiLog::LogSink());

    EXPECT_EQ(calls, 2);
    EXPECT_TRUE(other.lines().empty());
    EXPECT_NE(err.find("a sink set the sink from inside its own call"), std::string::npos);
}

TEST(Medium_LogReentry, ASinkThatSetsTheModeChangesNothing)
{
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink([](const SushiLog::LogRecord&)
    {
        SushiLog::set_log_synchronous(false);
    }));

    testing::internal::CaptureStderr();
    SAMPLE_LOG_WARN(K_LOOP_LOG_CATEGORY, "mode");
    const std::string err = testing::internal::GetCapturedStderr();
    SushiLog::set_log_sink(SushiLog::LogSink());

    EXPECT_NE(err.find("a sink set the delivery mode from inside its own call"),
              std::string::npos);
}
