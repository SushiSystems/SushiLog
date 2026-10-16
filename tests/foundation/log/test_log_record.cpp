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
