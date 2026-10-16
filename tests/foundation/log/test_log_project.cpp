/**************************************************************************/
/* test_log_project.cpp                                                   */
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
 * @file test_log_project.cpp
 * @brief Tests LogProject, which holds a project's name and its level variable.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_project.hpp>

#include <gtest/gtest.h>

namespace
{
    constexpr SushiLog::LogProject K_PROJECT{"sushisample", "SS_LOG_LEVEL"};
} // namespace

TEST(Small_LogProject, HoldsItsNameAndVariableAtCompileTime)
{
    static_assert(K_PROJECT.name() == "sushisample");
    static_assert(K_PROJECT.level_variable() == "SS_LOG_LEVEL");
    EXPECT_EQ(K_PROJECT.name(), "sushisample");
}
