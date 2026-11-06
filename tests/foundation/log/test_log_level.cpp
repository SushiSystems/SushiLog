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
 * @file test_log_level.cpp
 * @brief Tests the numbers of LogLevel, which the C interface fixes.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_level.hpp>

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
