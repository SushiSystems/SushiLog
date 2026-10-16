/**************************************************************************/
/* test_log_level.cpp                                                     */
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
 * @file test_log_level.cpp
 * @brief Tests the numbers of LogLevel, which the C interface fixes.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_level.hpp>

#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>

TEST(Small_LogLevel, NumbersRunFromTraceToOff)
{
    EXPECT_EQ(static_cast<int>(SushiLog::LogLevel::Trace), 0);
    EXPECT_EQ(static_cast<int>(SushiLog::LogLevel::Debug), 1);
    EXPECT_EQ(static_cast<int>(SushiLog::LogLevel::Info), 2);
    EXPECT_EQ(static_cast<int>(SushiLog::LogLevel::Warn), 3);
    EXPECT_EQ(static_cast<int>(SushiLog::LogLevel::Error), 4);
    EXPECT_EQ(static_cast<int>(SushiLog::LogLevel::Fatal), 5);
    EXPECT_EQ(static_cast<int>(SushiLog::LogLevel::Off), 6);
}

TEST(Small_LogLevel, IsOneByte)
{
    EXPECT_TRUE((std::is_same_v<std::underlying_type_t<SushiLog::LogLevel>, std::uint8_t>));
}
