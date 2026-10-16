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
 * @file test_environment.cpp
 * @brief Tests that a project's level variable sets its levels at its first use, and only its.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <string>

#include <gtest/gtest.h>

#include "environment_variable.hpp"

namespace
{
    using SushiLog::LogLevel;
} // namespace

TEST(Small_LogEnvironment, AVariableSetsEveryModuleAndThenEachPair)
{
    SushiLogTests::set_environment_variable("ENVPAIRS_LOG_LEVEL", "debug,autograd=trace");
    constexpr SushiLog::LogProject project{"envpairs", "ENVPAIRS_LOG_LEVEL"};

    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(project, "autograd")),
              LogLevel::Trace);
    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(project, "tensor")),
              LogLevel::Debug);
}

TEST(Small_LogEnvironment, AVariableIsReadInAnyCaseAndWithSpaces)
{
    SushiLogTests::set_environment_variable("ENVCASE_LOG_LEVEL", " OFF , Graph = Error ");
    constexpr SushiLog::LogProject project{"envcase", "ENVCASE_LOG_LEVEL"};

    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(project, "graph")),
              LogLevel::Error);
    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(project, "other")), LogLevel::Off);
}

TEST(Small_LogEnvironment, AVariableTouchesNoOtherProject)
{
    SushiLogTests::set_environment_variable("ENVMINE_LOG_LEVEL", "off");
    constexpr SushiLog::LogProject mine{"envmine", "ENVMINE_LOG_LEVEL"};
    constexpr SushiLog::LogProject theirs{"envtheirs", "ENVTHEIRS_LOG_LEVEL"};

    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(mine, "a")), LogLevel::Off);
    EXPECT_EQ(SushiLog::log_level(SushiLog::make_log_category(theirs, "a")), LogLevel::Warn);
}

TEST(Small_LogEnvironment, ACallBeforeTheFirstLogCallWinsOverTheVariable)
{
    SushiLogTests::set_environment_variable("ENVFIRST_LOG_LEVEL", "trace");
    constexpr SushiLog::LogProject project{"envfirst", "ENVFIRST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory category = SushiLog::make_log_category(project, "io");

    SushiLog::set_log_level(category, LogLevel::Error);

    EXPECT_EQ(SushiLog::log_level(category), LogLevel::Error);
}

TEST(Medium_LogEnvironment, AnUnreadableValueIsReportedOnceAndLeavesWarn)
{
    SushiLogTests::set_environment_variable("ENVBAD_LOG_LEVEL", "loud,graph=");
    constexpr SushiLog::LogProject project{"envbad", "ENVBAD_LOG_LEVEL"};

    testing::internal::CaptureStderr();
    const LogLevel first = SushiLog::log_level(SushiLog::make_log_category(project, "graph"));
    const LogLevel second = SushiLog::log_level(SushiLog::make_log_category(project, "other"));
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_EQ(first, LogLevel::Warn);
    EXPECT_EQ(second, LogLevel::Warn);
    const std::size_t at = err.find("ENVBAD_LOG_LEVEL='loud,graph='");
    ASSERT_NE(at, std::string::npos);
    EXPECT_EQ(err.find("ENVBAD_LOG_LEVEL=", at + 1), std::string::npos);
}
