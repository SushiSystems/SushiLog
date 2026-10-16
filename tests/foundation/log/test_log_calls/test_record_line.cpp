/**************************************************************************/
/* test_record_line.cpp                                                   */
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
 * @file test_record_line.cpp
 * @brief Tests the line of a record and the fields a sink receives with it.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <chrono>
#include <cstdint>
#include <regex>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "recording_sink.hpp"
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_LINE_PROJECT{"linetest", "LINETEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_MEMORY_LOG_CATEGORY =
        SushiLog::make_log_category(K_LINE_PROJECT, "memory");

    /** @brief Returns the system clock now in nanoseconds since the Unix epoch. */
    std::int64_t now_ns()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }
} // namespace

TEST(Small_LogRecordLine, LineHoldsTimeLevelCategoryThreadCallSiteAndMessage)
{
    const SushiLogTests::SynchronousRecording recording;

    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "share refused for {} MiB", 64);

    ASSERT_EQ(recording.sink().lines().size(), 1u);
    const SushiLogTests::RecordedLine line = recording.sink().lines()[0];
    const std::regex shape(R"(^\[\d\d:\d\d:\d\d\.\d{3}\] \[WARN\] \[linetest\.memory\] \[t\d+\] )"
                           R"(\[test_record_line\.cpp:\d+\] share refused for 64 MiB$)");
    EXPECT_TRUE(std::regex_match(line.text, shape)) << line.text;
    EXPECT_EQ(line.message, "share refused for 64 MiB");
    EXPECT_EQ(line.file, "test_record_line.cpp");
    EXPECT_EQ(line.project, "linetest");
    EXPECT_EQ(line.module, "memory");
}

TEST(Small_LogRecordLine, TimeIsTheSystemClockOfTheCall)
{
    const SushiLogTests::SynchronousRecording recording;
    const std::int64_t before = now_ns();

    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "timed");

    const std::int64_t after = now_ns();
    ASSERT_EQ(recording.sink().lines().size(), 1u);
    EXPECT_GE(recording.sink().lines()[0].time_ns, before);
    EXPECT_LE(recording.sink().lines()[0].time_ns, after);
}

TEST(Small_LogRecordLine, ALineBreakInTheMessageBecomesASpaceInTheLine)
{
    const SushiLogTests::SynchronousRecording recording;

    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "first\nsecond\r\nthird");

    ASSERT_EQ(recording.sink().lines().size(), 1u);
    const std::string text = recording.sink().lines()[0].text;
    EXPECT_EQ(text.find('\n'), std::string::npos);
    EXPECT_EQ(text.find('\r'), std::string::npos);
    EXPECT_NE(text.find("first second  third"), std::string::npos);
}

TEST(Small_LogRecordLine, OneThreadKeepsOneNumber)
{
    const SushiLogTests::SynchronousRecording recording;

    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "one");
    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "two");

    ASSERT_EQ(recording.sink().lines().size(), 2u);
    EXPECT_EQ(recording.sink().lines()[0].thread, recording.sink().lines()[1].thread);
}

TEST(Medium_LogRecordLine, TwoThreadsGetTwoNumbers)
{
    const SushiLogTests::SynchronousRecording recording;

    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "main");
    std::thread other([]()
    {
        SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "other");
    });
    other.join();

    ASSERT_EQ(recording.sink().lines().size(), 2u);
    EXPECT_NE(recording.sink().lines()[0].thread, recording.sink().lines()[1].thread);
}
