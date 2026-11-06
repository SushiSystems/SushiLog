/**************************************************************************/
/* test_levels.cpp                                                        */
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
 * @file test_levels.cpp
 * @brief Tests set_log_level and log_level for projects and categories, and project ownership.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <string>

#include <gtest/gtest.h>

#include <SushiLog/log/log_capture.hpp>
#include "sample_front.hpp"

namespace
{
    using SushiLog::LogLevel;

    constexpr SushiLog::LogProject K_LEVEL_PROJECT{"leveltest", "LEVELTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_GRAPH_LOG_CATEGORY =
        SushiLog::make_log_category(K_LEVEL_PROJECT, "graph");
    constexpr SushiLog::LogCategory K_DEVICE_LOG_CATEGORY =
        SushiLog::make_log_category(K_LEVEL_PROJECT, "device");

    constexpr SushiLog::LogProject K_OTHER_PROJECT{"othertest", "OTHERTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_OTHER_LOG_CATEGORY =
        SushiLog::make_log_category(K_OTHER_PROJECT, "graph");
} // namespace

TEST(Small_LogLevels, ACategoryNothingSetsIsAtWarn)
{
    constexpr SushiLog::LogProject project{"defaulttest", "DEFAULTTEST_LOG_LEVEL"};

    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(project, "any")), LogLevel::Warn);
    EXPECT_EQ(SushiLog::log_level(project), LogLevel::Warn);
}

TEST(Small_LogLevels, SettingAProjectSetsEveryCategoryOfIt)
{
    static_cast<void>(SushiLog::log_level(K_GRAPH_LOG_CATEGORY));

    SushiLog::set_log_level(K_LEVEL_PROJECT, LogLevel::Debug);

    EXPECT_EQ(SushiLog::log_level(K_LEVEL_PROJECT), LogLevel::Debug);
    EXPECT_EQ(SushiLog::log_level(K_GRAPH_LOG_CATEGORY), LogLevel::Debug);
    EXPECT_EQ(SushiLog::log_level(K_DEVICE_LOG_CATEGORY), LogLevel::Debug);
}

TEST(Small_LogLevels, ACategorySetAloneKeepsItsLevelWhenTheProjectIsSet)
{
    SushiLog::set_log_level(K_DEVICE_LOG_CATEGORY, LogLevel::Trace);

    SushiLog::set_log_level(K_LEVEL_PROJECT, LogLevel::Error);

    EXPECT_EQ(SushiLog::log_level(K_DEVICE_LOG_CATEGORY), LogLevel::Trace);
    EXPECT_EQ(SushiLog::log_level(K_GRAPH_LOG_CATEGORY), LogLevel::Error);
}

TEST(Small_LogLevels, SettingAProjectTouchesNoOtherProject)
{
    static_cast<void>(SushiLog::log_level(K_OTHER_LOG_CATEGORY));
    static_cast<void>(SushiLog::log_level(K_GRAPH_LOG_CATEGORY));

    SushiLog::set_log_level(K_LEVEL_PROJECT, LogLevel::Off);

    EXPECT_EQ(SushiLog::log_level(K_OTHER_LOG_CATEGORY), LogLevel::Warn);
}

TEST(Small_LogLevels, AProjectSetBeforeItsFirstCategoryAppliesToIt)
{
    const SushiLog::LogCapture recording;
    constexpr SushiLog::LogProject early{"earlytest", "EARLYTEST_LOG_LEVEL"};

    SushiLog::set_log_level(early, LogLevel::Trace);

    EXPECT_TRUE(recording.records().empty());
    EXPECT_EQ(SushiLog::log_level(early), LogLevel::Trace);
    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(early, "later")), LogLevel::Trace);
}

TEST(Small_LogLevels, ReadingAnUnseenProjectGivesItsDefaultAndWritesNothing)
{
    const SushiLog::LogCapture recording;
    constexpr SushiLog::LogProject unread{"unreadtest", "UNREADTEST_LOG_LEVEL"};

    EXPECT_EQ(SushiLog::log_level(unread), LogLevel::Warn);
    EXPECT_TRUE(recording.records().empty());
}

TEST(Small_LogLevels, ALogCallPassesAtOrAboveItsCategoryLevel)
{
    const SushiLog::LogCapture recording;
    SushiLog::set_log_level(K_GRAPH_LOG_CATEGORY, LogLevel::Info);

    SAMPLE_LOG_DEBUG(K_GRAPH_LOG_CATEGORY, "below");
    SAMPLE_LOG_INFO(K_GRAPH_LOG_CATEGORY, "at");
    SAMPLE_LOG_ERROR(K_GRAPH_LOG_CATEGORY, "above");

    EXPECT_EQ(recording.count_of("below"), 0u);
    EXPECT_EQ(recording.count_of("at"), 1u);
    EXPECT_EQ(recording.count_of("above"), 1u);
}

TEST(Small_LogLevels, LogAtWritesTheGivenCallSitePastEveryLevel)
{
    const SushiLog::LogCapture recording;
    SushiLog::set_log_level(K_GRAPH_LOG_CATEGORY, LogLevel::Off);

    SushiLog::log_at(LogLevel::Error, K_GRAPH_LOG_CATEGORY, "failing.cpp", 42, "check failed");

    ASSERT_EQ(recording.records().size(), 1u);
    const SushiLog::CapturedRecord line = recording.records()[0];
    EXPECT_EQ(line.file, "failing.cpp");
    EXPECT_EQ(line.line, 42u);
    EXPECT_EQ(line.message, "check failed");
}
