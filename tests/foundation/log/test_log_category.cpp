/**************************************************************************/
/* test_log_category.cpp                                                  */
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
 * @file test_log_category.cpp
 * @brief Tests LogCategory and make_log_category.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_category.hpp>

#include <gtest/gtest.h>

namespace
{
    constexpr SushiLog::LogProject K_PROJECT{"sushisample", "SS_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_GRAPH_LOG_CATEGORY =
        SushiLog::make_log_category(K_PROJECT, "graph");
} // namespace

TEST(Small_LogCategory, HoldsItsProjectAndModuleAtCompileTime)
{
    static_assert(K_GRAPH_LOG_CATEGORY.module() == "graph");
    static_assert(K_GRAPH_LOG_CATEGORY.project().name() == "sushisample");
    EXPECT_EQ(K_GRAPH_LOG_CATEGORY.project().level_variable(), "SS_LOG_LEVEL");
}
