/**************************************************************************/
/* test_fatal.cpp                                                         */
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
 * @file test_fatal.cpp
 * @brief Tests that FATAL is delivered on the calling thread after every queued record.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "delivery_mode_guard.hpp"
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_FATAL_PROJECT{"fataltest", "FATALTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_DEVICE_LOG_CATEGORY =
        SushiLog::make_log_category(K_FATAL_PROJECT, "device");

    /** @brief Holds what a sink saw: each message and the thread it was delivered on. */
    struct Delivery
    {
        std::mutex mutex;                    /**< Guards both lists. */
        std::vector<std::string> messages;   /**< The messages, in delivery order. */
        std::vector<std::thread::id> threads; /**< The thread of each delivery. */
    };
} // namespace

TEST(Medium_LogFatal, FatalFollowsTheQueueOnTheCallingThread)
{
    Delivery delivery;
    const SushiLogTests::SinkGuard sink(
        SushiLog::LogSink([&delivery](const SushiLog::LogRecord& record)
        {
            std::lock_guard<std::mutex> lock(delivery.mutex);
            delivery.messages.emplace_back(record.message);
            delivery.threads.push_back(std::this_thread::get_id());
        }));
    const SushiLogTests::DeliveryModeGuard mode(false);

    SAMPLE_LOG_WARN(K_DEVICE_LOG_CATEGORY, "queued before");
    SAMPLE_LOG_FATAL(K_DEVICE_LOG_CATEGORY, "device lost with no recovery path");

    std::vector<std::string> messages;
    std::vector<std::thread::id> threads;
    {
        std::lock_guard<std::mutex> lock(delivery.mutex);
        messages = delivery.messages;
        threads = delivery.threads;
    }
    ASSERT_EQ(messages.size(), 2u);
    EXPECT_EQ(messages[0], "queued before");
    EXPECT_EQ(messages[1], "device lost with no recovery path");
    EXPECT_EQ(threads[1], std::this_thread::get_id());
}
