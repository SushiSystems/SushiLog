/**************************************************************************/
/* test_value_text.cpp                                                    */
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
 * @file test_value_text.cpp
 * @brief Tests fixed, general, scientific, hex, right and left.
 * @author Mustafa Garip
 * @see tests/core/text/README.md
 */

#include <SushiLog/text/text_format.hpp>
#include <SushiLog/text/value_text.hpp>

#include <gtest/gtest.h>

TEST(Small_ValueText, FixedWritesThePrecisionAsked)
{
    EXPECT_EQ(SushiLog::fixed(1.23456, 2), "1.23");
    EXPECT_EQ(SushiLog::fixed(2.0, 0), "2");
}

TEST(Small_ValueText, GeneralTakesTheShorterForm)
{
    EXPECT_EQ(SushiLog::general(0.5), "0.5");
    EXPECT_EQ(SushiLog::general(1234567.0), "1.23457e+06");
    EXPECT_EQ(SushiLog::general(3.14159265, 3), "3.14");
}

TEST(Small_ValueText, ScientificWritesThePrecisionAsked)
{
    EXPECT_EQ(SushiLog::scientific(12345.678, 3), "1.235e+04");
}

TEST(Small_ValueText, HexIsLowerCaseAndZeroPadded)
{
    EXPECT_EQ(SushiLog::hex(0x0803, 8), "00000803");
    EXPECT_EQ(SushiLog::hex(0x1234567890, 4), "1234567890");
}

TEST(Small_ValueText, RightAndLeftAlignAnyStreamableValue)
{
    EXPECT_EQ(SushiLog::right(42, 5), "   42");
    EXPECT_EQ(SushiLog::left("ab", 4), "ab  ");
    EXPECT_EQ(SushiLog::right("abcdef", 3), "abcdef");
}

TEST(Small_ValueText, HelpersComposeAndFeedAFormat)
{
    EXPECT_EQ(SushiLog::right(SushiLog::fixed(3.14159, 2), 8), "    3.14");
    EXPECT_EQ(SushiLog::format_text("loss {}", SushiLog::fixed(0.5, 2)), "loss 0.50");
}
