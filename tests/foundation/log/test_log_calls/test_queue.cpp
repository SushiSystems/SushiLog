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

#include "delivery_mode_guard.hpp"
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

    /// @brief The number of records the queue of the logger holds.
    constexpr int K_CAPACITY = 4096;

    /// @brief The number of records logged after the queue is full.
    constexpr int K_OVERFLOW = 10;

    /** @brief Logs K_CAPACITY + K_OVERFLOW numbered records, "flood 0" on. */
    void fill_the_queue_past_capacity()
    {
        for (int i = 0; i < K_CAPACITY + K_OVERFLOW; ++i)
            SAMPLE_LOG_WARN(K_FLOOD_LOG_CATEGORY, "flood {}", i);
    }

    /** @brief Waits until the sink of @p gate holds its first record. */
    void wait_until_the_sink_holds_a_record(Gate& gate)
    {
        std::unique_lock<std::mutex> lock(gate.mutex);
        gate.changed.wait(lock, [&gate]()
        {
            return gate.entered;
        });
    }

    /** @brief Lets the sink of @p gate go. */
    void release_the_sink(Gate& gate)
    {
        {
            std::lock_guard<std::mutex> lock(gate.mutex);
            gate.released = true;
        }
        gate.changed.notify_all();
    }
} // namespace

TEST(Medium_LogQueue, AFullQueueDropsCountsAndReportsBeforeTheNextRecord)
{
    Gate gate;
    const SushiLogTests::SinkGuard sink(
        SushiLog::LogSink([&gate](const SushiLog::LogRecord& record)
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
    const SushiLogTests::DeliveryModeGuard mode(false);

    SAMPLE_LOG_WARN(K_FLOOD_LOG_CATEGORY, "first");
    wait_until_the_sink_holds_a_record(gate);
    fill_the_queue_past_capacity();
    release_the_sink(gate);
    SushiLog::set_log_synchronous(true);

    ASSERT_EQ(gate.seen.size(), static_cast<std::size_t>(K_CAPACITY) + 2u);
    EXPECT_EQ(gate.seen[0], "first");
    EXPECT_EQ(gate.seen[1], "dropped 10 records: the log queue of 4096 was full; the records "
                            "after them are delivered");
    EXPECT_EQ(gate.seen[2], "flood 0");
    EXPECT_EQ(gate.seen.back(), "flood " + std::to_string(K_CAPACITY - 1));
}
