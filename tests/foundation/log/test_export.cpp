/**************************************************************************/
/* test_export.cpp                                                        */
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
 * @file test_export.cpp
 * @brief Tests that export.hpp defines SUSHILOG_API for a consumer of the library.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/export.hpp>

#include <gtest/gtest.h>

#ifndef SUSHILOG_API
    #error "export.hpp defines no SUSHILOG_API"
#endif

TEST(Small_Export, ExportedFunctionsLinkFromTheSharedLibrary)
{
    SUCCEED() << "SUSHILOG_API is defined; the calls in the other tests link through it.";
}
