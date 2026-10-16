/**************************************************************************/
/* test_queue.cpp                                                         */
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
 * @file test_queue.cpp
 * @brief Tests the bounded queue: a record that finds it full is dropped, counted and reported.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "recording_sink.hpp"
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_QUEUE_PROJECT{"queuetest", "QUEUETEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_FLOOD_LOG_CATEGORY =
        SushiLog::make_log_category(K_QUEUE_PROJECT, "flood");

    /** @brief Holds the first delivery inside the sink until the test releases it. */
    struct Gate
    {
        std::mutex mutex;                 /**< Guards the two flags. */
        std::condition_variable changed;  /**< Wakes a waiter on either flag. */
        bool entered = false;             /**< True once the sink holds the first record. */
        bool released = false;            /**< True once the test lets it go. */
        std::vector<std::string> seen;    /**< The messages delivered, in order. */
    };

    /** @brief The number of records the queue of the logger holds. */
    constexpr int K_CAPACITY = 4096;
} // namespace

TEST(Medium_LogQueue, AFullQueueDropsCountsAndReportsBeforeTheNextRecord)
{
    Gate gate;
    SushiLog::set_log_sink(SushiLog::LogSink([&gate](const SushiLog::LogRecord& record)
    {
        std::unique_lock<std::mutex> lock(gate.mutex);
        gate.seen.emplace_back(record.message);
        if (!gate.entered)
        {
            gate.entered = true;
            gate.changed.notify_all();
            gate.changed.wait(lock, [&gate]()
            {
                return gate.released;
            });
        }
    }));
    SushiLog::set_log_synchronous(false);

    SAMPLE_LOG_WARN(K_FLOOD_LOG_CATEGORY, "first");
    {
        std::unique_lock<std::mutex> lock(gate.mutex);
        gate.changed.wait(lock, [&gate]()
        {
            return gate.entered;
        });
    }
    for (int i = 0; i < K_CAPACITY + 10; ++i)
        SAMPLE_LOG_WARN(K_FLOOD_LOG_CATEGORY, "flood {}", i);
    {
        std::lock_guard<std::mutex> lock(gate.mutex);
        gate.released = true;
    }
    gate.changed.notify_all();
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink());

    ASSERT_EQ(gate.seen.size(), static_cast<std::size_t>(K_CAPACITY) + 2u);
    EXPECT_EQ(gate.seen[0], "first");
    EXPECT_EQ(gate.seen[1], "dropped 10 records: the log queue of 4096 was full; the records "
                            "after them are delivered");
    EXPECT_EQ(gate.seen[2], "flood 0");
    EXPECT_EQ(gate.seen.back(), "flood " + std::to_string(K_CAPACITY - 1));
}
