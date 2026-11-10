/**************************************************************************/
/* test_log_executor.cpp                                                  */
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
 * @file test_log_executor.cpp
 * @brief Tests the executor port: the writer posts one drain, and FATAL and failure drain inline.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_executor.hpp>
#include <SushiLog/log/log_manual.hpp>

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <SushiLog/log/log_capture.hpp>
#include "delivery_mode_guard.hpp"
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_MANUAL_PROJECT{"manualtest", "MANUALTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_MANUAL_LOG_CATEGORY =
        SushiLog::make_log_category(K_MANUAL_PROJECT, "manual");

    /** @brief Holds what a sink saw: each message and the time and thread of its delivery. */
    struct Delivery
    {
        std::mutex mutex;                       /**< Guards the lists. */
        std::vector<std::string> messages;      /**< The messages, in delivery order. */
        std::vector<std::int64_t> times;        /**< The time of each record. */
        std::vector<std::thread::id> threads;   /**< The thread of each delivery. */

        /** @brief Returns how many records arrived. */
        std::size_t count()
        {
            std::lock_guard<std::mutex> lock(mutex);
            return messages.size();
        }
    };

    /** @brief Returns a sink that notes each record in @p delivery. */
    SushiLog::LogSink noting_sink(Delivery& delivery)
    {
        return SushiLog::LogSink([&delivery](const SushiLog::LogRecord& record)
        {
            std::lock_guard<std::mutex> lock(delivery.mutex);
            delivery.messages.emplace_back(record.message);
            delivery.times.push_back(record.time_ns);
            delivery.threads.push_back(std::this_thread::get_id());
        });
    }
} // namespace

TEST(Small_LogExecutor, TheWriterPostsOneDrainAndTheExecutorRunsIt)
{
    Delivery delivery;
    SushiLog::ManualLogExecutor manual;
    const SushiLogTests::SinkGuard sink(noting_sink(delivery));
    const SushiLogTests::DeliveryModeGuard mode(false);
    const SushiLog::ScopedLogExecutor executor{manual.as_log_executor()};

    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "one");
    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "two");

    EXPECT_EQ(delivery.count(), 0u);
    EXPECT_EQ(manual.pending(), 1u);
    EXPECT_EQ(manual.run_pending(), 1u);
    ASSERT_EQ(delivery.count(), 2u);
    EXPECT_EQ(delivery.messages[0], "one");
    EXPECT_EQ(delivery.messages[1], "two");
    EXPECT_EQ(delivery.threads[0], std::this_thread::get_id());
}

TEST(Small_LogExecutor, FatalFollowsTheHeldRecordsOnTheCallingThread)
{
    Delivery delivery;
    SushiLog::ManualLogExecutor manual;
    SushiLog::ManualLogClock clock(42);
    const SushiLogTests::SinkGuard sink(noting_sink(delivery));
    const SushiLogTests::DeliveryModeGuard mode(false);
    const SushiLog::ScopedLogExecutor executor{manual.as_log_executor()};
    const SushiLog::ScopedLogClock timing{clock.as_log_clock()};

    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "held");
    SAMPLE_LOG_FATAL(K_MANUAL_LOG_CATEGORY, "lost");

    ASSERT_EQ(delivery.count(), 2u);
    EXPECT_EQ(delivery.messages[0], "held");
    EXPECT_EQ(delivery.messages[1], "lost");
    EXPECT_EQ(delivery.times[1], 42);
    EXPECT_EQ(delivery.threads[1], std::this_thread::get_id());
    static_cast<void>(manual.run_pending());
    EXPECT_EQ(delivery.count(), 2u);
}

TEST(Small_LogExecutor, SettingTheDefaultExecutorDeliversWhatTheOldOneHeld)
{
    Delivery delivery;
    SushiLog::ManualLogExecutor manual;
    const SushiLogTests::SinkGuard sink(noting_sink(delivery));
    const SushiLogTests::DeliveryModeGuard mode(false);

    SushiLog::set_log_executor(manual.as_log_executor());
    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "held");
    EXPECT_EQ(delivery.count(), 0u);
    SushiLog::set_log_executor(SushiLog::LogExecutor());
    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "after");
    SushiLog::set_log_synchronous(true);

    ASSERT_EQ(delivery.count(), 2u);
    EXPECT_EQ(delivery.messages[0], "held");
}

TEST(Small_LogExecutor, AThrowingExecutorLeavesTheDrainToTheCallingThread)
{
    Delivery delivery;
    const SushiLogTests::SinkGuard sink(noting_sink(delivery));
    const SushiLogTests::DeliveryModeGuard mode(false);
    const SushiLog::ScopedLogExecutor executor{SushiLog::LogExecutor([](SushiLog::LogTask)
    {
        throw std::runtime_error("executor refused");
    })};

    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "inline");

    ASSERT_EQ(delivery.count(), 1u);
    EXPECT_EQ(delivery.messages[0], "inline");
    EXPECT_EQ(delivery.threads[0], std::this_thread::get_id());
}

TEST(Small_LogExecutor, TheScopedExecutorRestoresTheDefaultOnExit)
{
    Delivery delivery;
    SushiLog::ManualLogExecutor manual;
    const SushiLogTests::SinkGuard sink(noting_sink(delivery));
    const SushiLogTests::DeliveryModeGuard mode(false);

    {
        const SushiLog::ScopedLogExecutor executor{manual.as_log_executor()};
        SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "held");
    }
    SAMPLE_LOG_WARN(K_MANUAL_LOG_CATEGORY, "after");
    SushiLog::set_log_synchronous(true);

    ASSERT_EQ(delivery.count(), 2u);
    manual.run_pending();
    EXPECT_EQ(delivery.count(), 2u);
}
