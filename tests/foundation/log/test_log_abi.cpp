/**************************************************************************/
/* test_log_abi.cpp                                                       */
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
 * @file test_log_abi.cpp
 * @brief Tests the ABI version and the check that stops a front built against another one.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_abi.hpp>

#include <gtest/gtest.h>

namespace
{
    constexpr SushiLog::LogProject K_ABI_PROJECT{"abitest", "ABITEST_LOG_LEVEL"};
} // namespace

TEST(Small_LogAbi, LibraryHoldsTheVersionItsHeadersName)
{
    EXPECT_EQ(SushiLog::abi_version(), SushiLog::K_ABI_VERSION);
}

TEST(Small_LogAbi, EqualVersionsPassTheCheck)
{
    SushiLog::Detail::check_log_abi(K_ABI_PROJECT, 3, 3);

    SUCCEED();
}

TEST(Medium_LogAbi, AMismatchNamesTheProjectBothVersionsAndTheLibraryThenEnds)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    EXPECT_DEATH(SushiLog::Detail::check_log_abi(K_ABI_PROJECT, 1, 2),
                 "abitest was compiled against SushiLog ABI 1, and the loaded .+ has ABI 2");
}
