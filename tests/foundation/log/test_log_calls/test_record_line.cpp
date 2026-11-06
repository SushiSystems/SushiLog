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
 * @file test_record_line.cpp
 * @brief Tests the line of a record and the fields a sink receives with it.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <regex>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include <SushiLog/log/log_capture.hpp>
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_LINE_PROJECT{"linetest", "LINETEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_MEMORY_LOG_CATEGORY =
        SushiLog::make_log_category(K_LINE_PROJECT, "memory");
} // namespace

TEST(Small_LogRecordLine, LineHoldsTimeLevelCategoryThreadCallSiteAndMessage)
{
    const SushiLog::LogCapture recording;

    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "share refused for {} MiB", 64);

    ASSERT_EQ(recording.records().size(), 1u);
    const SushiLog::CapturedRecord line = recording.records()[0];
    const std::regex shape(R"(^\[\d\d:\d\d:\d\d\.\d{3}\] \[WARN\] \[linetest\.memory\] \[t\d+\] )"
                           R"(\[test_record_line\.cpp:\d+\] share refused for 64 MiB$)");
    EXPECT_TRUE(std::regex_match(line.text, shape)) << line.text;
    EXPECT_EQ(line.message, "share refused for 64 MiB");
    EXPECT_EQ(line.file, "test_record_line.cpp");
    EXPECT_EQ(line.project, "linetest");
    EXPECT_EQ(line.module, "memory");
}

TEST(Small_LogRecordLine, ALineBreakInTheMessageBecomesASpaceInTheLine)
{
    const SushiLog::LogCapture recording;

    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "first\nsecond\r\nthird");

    ASSERT_EQ(recording.records().size(), 1u);
    const std::string text = recording.records()[0].text;
    EXPECT_EQ(text.find('\n'), std::string::npos);
    EXPECT_EQ(text.find('\r'), std::string::npos);
    EXPECT_NE(text.find("first second  third"), std::string::npos);
}

TEST(Medium_LogRecordLine, TwoThreadsGetTwoNumbers)
{
    const SushiLog::LogCapture recording;

    SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "main");
    std::thread other([]()
    {
        SAMPLE_LOG_WARN(K_MEMORY_LOG_CATEGORY, "other");
    });
    other.join();

    ASSERT_EQ(recording.records().size(), 2u);
    EXPECT_NE(recording.records()[0].thread, recording.records()[1].thread);
}
