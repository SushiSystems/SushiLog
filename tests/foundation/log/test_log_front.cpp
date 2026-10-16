/**************************************************************************/
/* test_log_front.cpp                                                     */
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
 * @file test_log_front.cpp
 * @brief Tests the two gates of SL_LOG_FRONT_AT: the compile-time ceiling and the run-time level.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_front.hpp>

#include <gtest/gtest.h>

#include <SushiLog/log/log_calls.hpp>

#include "recording_sink.hpp"

namespace
{
    constexpr SushiLog::LogProject K_FRONT_PROJECT{"fronttest", "FRONTTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_GATE_LOG_CATEGORY =
        SushiLog::make_log_category(K_FRONT_PROJECT, "gate");

    /** @brief Counts how often an argument of a log call was evaluated. */
    int g_evaluations = 0;

    /** @brief Returns @p value and counts the evaluation. */
    int counted(int value)
    {
        ++g_evaluations;
        return value;
    }
} // namespace

TEST(Small_LogFront, CeilingIsErrorWhenLoggingIsOff)
{
    static_assert(SushiLog::log_level_ceiling(false) == SushiLog::LogLevel::Error);
#ifdef NDEBUG
    static_assert(SushiLog::log_level_ceiling(true) == SushiLog::LogLevel::Debug);
#else
    static_assert(SushiLog::log_level_ceiling(true) == SushiLog::LogLevel::Trace);
#endif
    SUCCEED();
}

TEST(Small_LogFront, ACallPastTheCeilingEvaluatesNothing)
{
    const SushiLogTests::SynchronousRecording recording;
    SushiLog::set_log_level(K_GATE_LOG_CATEGORY, SushiLog::LogLevel::Trace);
    g_evaluations = 0;

    SL_LOG_FRONT_AT(SushiLog::log_level_ceiling(false), SushiLog::LogLevel::Warn,
                    K_GATE_LOG_CATEGORY, "value {}", counted(1));

    EXPECT_EQ(g_evaluations, 0);
    EXPECT_TRUE(recording.sink().lines().empty());
}

TEST(Small_LogFront, ACallBelowTheCategoryLevelEvaluatesNothing)
{
    const SushiLogTests::SynchronousRecording recording;
    SushiLog::set_log_level(K_GATE_LOG_CATEGORY, SushiLog::LogLevel::Warn);
    g_evaluations = 0;

    SL_LOG_FRONT_AT(SushiLog::LogLevel::Trace, SushiLog::LogLevel::Debug, K_GATE_LOG_CATEGORY,
                    "value {}", counted(1));

    EXPECT_EQ(g_evaluations, 0);
    EXPECT_TRUE(recording.sink().lines().empty());
}

TEST(Small_LogFront, ACallPastBothGatesIsDeliveredOnce)
{
    const SushiLogTests::SynchronousRecording recording;
    SushiLog::set_log_level(K_GATE_LOG_CATEGORY, SushiLog::LogLevel::Warn);
    g_evaluations = 0;

    SL_LOG_FRONT_AT(SushiLog::LogLevel::Trace, SushiLog::LogLevel::Error, K_GATE_LOG_CATEGORY,
                    "value {}", counted(4));

    EXPECT_EQ(g_evaluations, 1);
    ASSERT_EQ(recording.sink().lines().size(), 1u);
    EXPECT_EQ(recording.sink().lines()[0].message, "value 4");
    EXPECT_EQ(recording.sink().lines()[0].level, SushiLog::LogLevel::Error);
}
