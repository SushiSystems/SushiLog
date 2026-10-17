/**************************************************************************/
/* test_text_format.cpp                                                   */
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
 * @file test_text_format.cpp
 * @brief Tests format_text: placeholders, literal braces and argument counts.
 * @author Mustafa Garip
 * @see tests/core/text/README.md
 */

#include <SushiLog/text/text_format.hpp>

#include <gtest/gtest.h>

using SushiLog::format_text;

TEST(Small_TextFormat, EachPlaceholderTakesTheNextArgument)
{
    EXPECT_EQ(format_text("{} of {}", 3, "four"), "3 of four");
}

TEST(Small_TextFormat, DoubledBracesAreWrittenOnce)
{
    EXPECT_EQ(format_text("{{}} and {}", 1), "{} and 1");
    EXPECT_EQ(format_text("{{only}}"), "{only}");
}

TEST(Small_TextFormat, APlaceholderPastTheLastArgumentStays)
{
    EXPECT_EQ(format_text("{} {}", 1), "1 {}");
}

TEST(Small_TextFormat, AnArgumentPastTheLastPlaceholderIsDropped)
{
    EXPECT_EQ(format_text("{}", 1, 2), "1");
}
