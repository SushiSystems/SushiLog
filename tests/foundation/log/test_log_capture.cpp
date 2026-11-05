/**************************************************************************/
/* test_log_capture.cpp                                                   */
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
 * @file test_log_capture.cpp
 * @brief Tests LogRecorder and LogCapture: what a capture sees and what it puts back on exit.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_capture.hpp>

#include <gtest/gtest.h>

#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_CAPTURE_PROJECT{"capturetest", "CAPTURETEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_SCOPE_LOG_CATEGORY =
        SushiLog::make_log_category(K_CAPTURE_PROJECT, "scope");
} // namespace

TEST(Small_LogCapture, HoldsARecordBeforeItsLogCallReturns)
{
    const SushiLog::LogCapture capture;

    SAMPLE_LOG_WARN(K_SCOPE_LOG_CATEGORY, "held");

    EXPECT_EQ(capture.count_of("held"), 1u);
}

TEST(Small_LogCapture, KeepsTheFieldsOfTheRecord)
{
    const SushiLog::LogCapture capture;

    SAMPLE_LOG_ERROR(K_SCOPE_LOG_CATEGORY, "fields");

    ASSERT_EQ(capture.records().size(), 1u);
    EXPECT_EQ(capture.records()[0].level, SushiLog::LogLevel::Error);
    EXPECT_EQ(capture.records()[0].project, "capturetest");
    EXPECT_EQ(capture.records()[0].module, "scope");
    EXPECT_EQ(capture.records()[0].message, "fields");
}

TEST(Small_LogCapture, PutsBackTheSinkItFoundOnExit)
{
    const SushiLog::LogCapture outer;
    {
        const SushiLog::LogCapture inner;
        SAMPLE_LOG_WARN(K_SCOPE_LOG_CATEGORY, "inner");
    }

    SAMPLE_LOG_WARN(K_SCOPE_LOG_CATEGORY, "after");

    EXPECT_EQ(outer.count_of("inner"), 0u);
    EXPECT_EQ(outer.count_of("after"), 1u);
}

TEST(Medium_LogCapture, PutsBackTheDeliveryModeItFoundOnExit)
{
    SushiLog::set_log_synchronous(false);
    {
        const SushiLog::LogCapture capture;
    }

    EXPECT_FALSE(SushiLog::log_synchronous());
}
