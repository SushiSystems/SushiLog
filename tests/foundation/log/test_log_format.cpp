/**************************************************************************/
/* test_log_format.cpp                                                    */
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
 * @file test_log_format.cpp
 * @brief Tests format_log_message: placeholders, doubled braces and surplus arguments.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_format.hpp>

#include <string>

#include <gtest/gtest.h>

using SushiLog::Detail::format_log_message;

TEST(Small_LogFormat, ReplacesEachPlaceholderInOrder)
{
    EXPECT_EQ(format_log_message("{} of {} at {}", 3, std::string("eight"), 1.5),
              "3 of eight at 1.5");
}

TEST(Small_LogFormat, WritesDoubledBracesOnce)
{
    EXPECT_EQ(format_log_message("{{}} {}", 1), "{} 1");
    EXPECT_EQ(format_log_message("{{literal}}"), "{literal}");
}

TEST(Small_LogFormat, LeavesAPlaceholderWithoutArgumentAsWritten)
{
    EXPECT_EQ(format_log_message("{} and {}", 1), "1 and {}");
}

TEST(Small_LogFormat, IgnoresAnArgumentWithoutPlaceholder)
{
    EXPECT_EQ(format_log_message("none", 1, 2), "none");
}
