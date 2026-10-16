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
