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
 * @file test_text_format.cpp
 * @brief Tests format_text: placeholders, literal braces and argument counts.
 * @author Mustafa Garip
 * @see tests/core/text/README.md
 */

#include <SushiLog/text/text_format.hpp>

#include <cstdint>
#include <ostream>

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

TEST(Small_TextFormat, APlaceholderWithNoArgumentShowsAsAQuestion)
{
    EXPECT_EQ(format_text("{} {}", 1), "1 {?}");
}

TEST(Small_TextFormat, ArgumentsPastTheLastPlaceholderAreCounted)
{
    EXPECT_EQ(format_text("{}", 1, 2, 3), "1 {+2}");
}

namespace
{
    enum class Phase : signed char { Load = 3 };

    enum class Mode { Fast = 1 };

    std::ostream& operator<<(std::ostream& stream, Mode)
    {
        return stream << "fast-mode";
    }
}

TEST(Small_TextFormat, AnEightBitIntegerIsWrittenAsANumber)
{
    EXPECT_EQ(format_text("{} {}", static_cast<std::int8_t>(-5), static_cast<std::uint8_t>(200)), "-5 200");
}

TEST(Small_TextFormat, AScopedEnumWithoutAnOutputOperatorIsWrittenAsItsValue)
{
    EXPECT_EQ(format_text("{}", Phase::Load), "3");
}

TEST(Small_TextFormat, AScopedEnumWithAnOutputOperatorIsWrittenThroughIt)
{
    EXPECT_EQ(format_text("{}", Mode::Fast), "fast-mode");
}

TEST(Small_TextFormat, ANullCStringIsWrittenAsNull)
{
    const char* missing = nullptr;
    EXPECT_EQ(format_text("[{}]", missing), "[(null)]");
}
