/**************************************************************************/
/* test_SushiLog.cpp                                                      */
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
 * @file test_SushiLog.cpp
 * @brief Tests that the umbrella header gives every public C++ name and leaves the C header out.
 * @author Mustafa Garip
 * @see tests/foundation/umbrella/README.md
 */

#include <SushiLog/SushiLog.hpp>

#include <gtest/gtest.h>

/**
 * @brief Holds a type of the C header's name; it compiles only while the umbrella leaves
 *        log_c_sink.h out.
 */
struct SushiLogRecord
{
    int declared_here; /**< A field the C header's struct does not have. */
};

namespace
{
    constexpr SushiLog::LogProject K_UMBRELLA_PROJECT{"umbrellatest", "UMBRELLATEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_USE_LOG_CATEGORY =
        SushiLog::make_log_category(K_UMBRELLA_PROJECT, "use");
} // namespace

TEST(Small_SushiLog, UmbrellaNamesTheLevelsTheSinkAndTheFront)
{
    SushiLog::set_log_synchronous(true);
    int delivered = 0;
    SushiLog::set_log_sink(SushiLog::LogSink([&delivered](const SushiLog::LogRecord&)
    {
        ++delivered;
    }));

    SL_LOG_FRONT_AT(SushiLog::log_level_ceiling(true), SushiLog::LogLevel::Error,
                    K_USE_LOG_CATEGORY, "reached {}", 1);
    SushiLog::set_log_sink(SushiLog::LogSink());

    EXPECT_EQ(delivered, 1);
    EXPECT_EQ(SushiLog::abi_version(), SushiLog::K_ABI_VERSION);
}

TEST(Small_SushiLog, UmbrellaLeavesTheCInterfaceOut)
{
    const SushiLogRecord local{7};

    EXPECT_EQ(local.declared_here, 7);
}
