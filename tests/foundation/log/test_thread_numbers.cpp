/**************************************************************************/
/* test_thread_numbers.cpp                                                */
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
 * @file test_thread_numbers.cpp
 * @brief Tests ThreadNumbers on plain identities.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/thread_numbers.hpp>

#include <gtest/gtest.h>

TEST(Small_ThreadNumbers, TheFirstIdentitySeenIsNumberOne)
{
    SushiLog::ThreadNumbers<int> numbers;

    EXPECT_EQ(numbers.number_of(70), 1u);
}

TEST(Small_ThreadNumbers, TwoIdentitiesGetTwoNumbersInTheOrderFirstSeen)
{
    SushiLog::ThreadNumbers<int> numbers;
    static_cast<void>(numbers.number_of(70));

    EXPECT_EQ(numbers.number_of(30), 2u);
}

TEST(Small_ThreadNumbers, OneIdentityKeepsItsNumber)
{
    SushiLog::ThreadNumbers<int> numbers;
    static_cast<void>(numbers.number_of(70));
    static_cast<void>(numbers.number_of(30));

    EXPECT_EQ(numbers.number_of(70), 1u);
}
