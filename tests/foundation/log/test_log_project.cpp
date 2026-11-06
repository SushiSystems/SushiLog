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
