/**************************************************************************/
/* test_log_record.cpp                                                    */
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
 * @file test_log_record.cpp
 * @brief Tests that LogRecord holds every field a sink reads.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_record.hpp>

#include <gtest/gtest.h>

TEST(Small_LogRecord, HoldsEachFieldApart)
{
    const SushiLog::LogRecord record{SushiLog::LogLevel::Error, "sushisample", "graph",
                                     "graph.cpp", 12, 3, 1700000000000000000, "failed",
                                     "[..] failed"};

    EXPECT_EQ(record.level, SushiLog::LogLevel::Error);
    EXPECT_EQ(record.project, "sushisample");
    EXPECT_EQ(record.module, "graph");
    EXPECT_EQ(record.file, "graph.cpp");
    EXPECT_EQ(record.line, 12u);
    EXPECT_EQ(record.thread, 3u);
    EXPECT_EQ(record.time_ns, 1700000000000000000);
    EXPECT_EQ(record.message, "failed");
    EXPECT_EQ(record.text, "[..] failed");
}
