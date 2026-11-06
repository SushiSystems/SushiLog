/**************************************************************************/
/* test_environment.cpp                                                   */
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
 * @file test_environment.cpp
 * @brief Tests that a project's level variable sets its levels at its first use, and only its.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <gtest/gtest.h>

#include "environment_variable.hpp"

namespace
{
    using SushiLog::LogLevel;
    using SushiLogTests::ScopedEnvironmentVariable;
} // namespace

TEST(Medium_LogEnvironment, AnExplicitLevelOverridesTheVariable)
{
    const ScopedEnvironmentVariable variable("SUSHILOGTEST_ENVFIRST_LOG_LEVEL", "trace");
    constexpr SushiLog::LogProject project{"envfirst", "SUSHILOGTEST_ENVFIRST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory category = SushiLog::make_log_category(project, "io");

    SushiLog::set_log_level(category, LogLevel::Error);

    EXPECT_EQ(SushiLog::log_level(category), LogLevel::Error);
}

TEST(Medium_LogEnvironment, AProjectsVariableSetsTheLevelsOfItsCategories)
{
    const ScopedEnvironmentVariable variable("SUSHILOGTEST_ENVWIRE_LOG_LEVEL",
                                             "debug,autograd=trace");
    constexpr SushiLog::LogProject project{"envwire", "SUSHILOGTEST_ENVWIRE_LOG_LEVEL"};

    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(project, "tensor")),
              LogLevel::Debug);
    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(project, "autograd")),
              LogLevel::Trace);
}
