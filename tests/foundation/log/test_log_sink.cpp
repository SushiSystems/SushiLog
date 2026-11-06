/**************************************************************************/
/* test_log_sink.cpp                                                      */
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
 * @file test_log_sink.cpp
 * @brief Tests LogSink, the callable type a sink is stored as.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_sink.hpp>

#include <string>

#include <gtest/gtest.h>

TEST(Small_LogSink, CallsItsCallableWithTheRecord)
{
    std::string seen;
    const SushiLog::LogSink sink([&seen](const SushiLog::LogRecord& record)
    {
        seen = std::string(record.message);
    });
    const SushiLog::LogRecord record{SushiLog::LogLevel::Warn, "p", "m", "f.cpp", 1, 1, 0,
                                     "hello", "line"};

    sink(record);

    EXPECT_EQ(seen, "hello");
}

TEST(Small_LogSink, AnEmptySinkIsFalse)
{
    const SushiLog::LogSink empty{};

    EXPECT_FALSE(empty);
}
