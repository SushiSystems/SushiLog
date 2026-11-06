/**************************************************************************/
/* test_level_setting.cpp                                                 */
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
 * @file test_level_setting.cpp
 * @brief Tests parse_level_setting, the grammar of a level variable's value.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/level_setting.hpp>

#include <optional>

#include <gtest/gtest.h>

namespace
{
    using SushiLog::LogLevel;
} // namespace

TEST(Small_LevelSetting, ALevelAloneSetsEveryModule)
{
    const std::optional<SushiLog::LevelSetting> setting = SushiLog::parse_level_setting("debug");

    ASSERT_TRUE(setting.has_value());
    EXPECT_EQ(setting->level_of("tensor"), LogLevel::Debug);
}

TEST(Small_LevelSetting, AModulePairOverridesTheLevelForThatModule)
{
    const std::optional<SushiLog::LevelSetting> setting =
        SushiLog::parse_level_setting("debug,autograd=trace");

    ASSERT_TRUE(setting.has_value());
    EXPECT_EQ(setting->level_of("autograd"), LogLevel::Trace);
}

TEST(Small_LevelSetting, AModuleNoPairNamesTakesTheLevel)
{
    const std::optional<SushiLog::LevelSetting> setting =
        SushiLog::parse_level_setting("debug,autograd=trace");

    ASSERT_TRUE(setting.has_value());
    EXPECT_EQ(setting->level_of("tensor"), LogLevel::Debug);
}

TEST(Small_LevelSetting, AValueWithNoLevelAloneLeavesWarn)
{
    const std::optional<SushiLog::LevelSetting> setting =
        SushiLog::parse_level_setting("graph=error");

    ASSERT_TRUE(setting.has_value());
    EXPECT_EQ(setting->level_of("other"), LogLevel::Warn);
}

TEST(Small_LevelSetting, CaseAndSpacesAroundItemsAreIgnored)
{
    const std::optional<SushiLog::LevelSetting> setting =
        SushiLog::parse_level_setting(" OFF , Graph = Error ");

    ASSERT_TRUE(setting.has_value());
    EXPECT_EQ(setting->level_of("graph"), LogLevel::Error);
    EXPECT_EQ(setting->level_of("other"), LogLevel::Off);
}

TEST(Small_LevelSetting, TheLastPairForAModuleWins)
{
    const std::optional<SushiLog::LevelSetting> setting =
        SushiLog::parse_level_setting("io=error,io=info");

    ASSERT_TRUE(setting.has_value());
    EXPECT_EQ(setting->level_of("io"), LogLevel::Info);
}

class Small_LevelSettingUnreadable : public testing::TestWithParam<const char*>
{
};

TEST_P(Small_LevelSettingUnreadable, AnUnreadableValueGivesNothing)
{
    EXPECT_FALSE(SushiLog::parse_level_setting(GetParam()).has_value());
}

INSTANTIATE_TEST_SUITE_P(Rows, Small_LevelSettingUnreadable,
                         testing::Values("loud", "loud,graph=", "debug,=trace", "debug,io=loud"));
