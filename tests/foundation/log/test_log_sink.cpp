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
