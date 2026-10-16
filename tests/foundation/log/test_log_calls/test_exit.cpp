/**************************************************************************/
/* test_exit.cpp                                                          */
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
 * @file test_exit.cpp
 * @brief Tests the logger across the life of the process: static initialisation and exit.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>

#include <gtest/gtest.h>

#include "sample_front.hpp"

namespace
{
    using SushiLog::LogLevel;

    constexpr SushiLog::LogProject K_EXIT_PROJECT{"exittest", "EXITTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_SHUTDOWN_LOG_CATEGORY =
        SushiLog::make_log_category(K_EXIT_PROJECT, "shutdown");

    /**
     * @brief Logs one record from its destructor once armed.
     *
     * It is constructed before the first use of the logger in this program, so its destructor
     * runs after the logger's thread has stopped, where the C++ runtime orders all static
     * destructors of a process in one list.
     */
    class ExitProbe final
    {
        public:
            ExitProbe() noexcept
            {
                armed_ = false;
            }

            ExitProbe(const ExitProbe&) = delete;
            ExitProbe& operator=(const ExitProbe&) = delete;
            ExitProbe(ExitProbe&&) = delete;
            ExitProbe& operator=(ExitProbe&&) = delete;

            ~ExitProbe()
            {
                if (armed_)
                    SAMPLE_LOG_ERROR(K_SHUTDOWN_LOG_CATEGORY, "logged during exit");
            }

            /** @brief Makes the destructor log. */
            void arm() noexcept
            {
                armed_ = true;
            }

        private:
            /// @brief True once arm() ran.
            bool armed_;
    };

    /// @brief Constructed before g_static_level in this file, so it is destroyed after it.
    ExitProbe g_exit_probe;

    /** @brief Sets and reads a category's level before main() runs: the first use. */
    LogLevel level_set_during_static_initialisation()
    {
        constexpr SushiLog::LogCategory startup =
            SushiLog::make_log_category(K_EXIT_PROJECT, "startup");
        SushiLog::set_log_level(startup, LogLevel::Info);
        return SushiLog::log_level(startup);
    }

    /** @brief Holds what level_set_during_static_initialisation() read. */
    const LogLevel g_static_level = level_set_during_static_initialisation();

    /** @brief Holds a sink's first delivery until the test releases it. */
    struct SinkGate
    {
        std::mutex mutex;                /**< Guards both flags. */
        std::condition_variable changed; /**< Wakes a waiter on either flag. */
        bool entered = false;            /**< True once the sink holds a record. */
        bool released = false;           /**< True once the test lets it go. */
    };

    /// @brief The gate of the child process of one death test.
    SinkGate g_gate;

    /** @brief Writes each record on stderr after the gate opens; the first call opens entry. */
    void gated_sink(const SushiLog::LogRecord& record)
    {
        std::unique_lock<std::mutex> lock(g_gate.mutex);
        g_gate.entered = true;
        g_gate.changed.notify_all();
        g_gate.changed.wait(lock, []()
        {
            return g_gate.released;
        });
        std::fprintf(stderr, "sink: %.*s\n", static_cast<int>(record.message.size()),
                     record.message.data());
    }

    /**
     * @brief Queues three records while the logger's thread is inside the sink, opens the
     *        gate when @p release is true, then exits.
     */
    [[noreturn]] void exit_with_records_queued(bool release)
    {
        SushiLog::set_log_sink(SushiLog::LogSink([](const SushiLog::LogRecord& record)
        {
            gated_sink(record);
        }));
        SushiLog::set_log_synchronous(false);
        for (int i = 1; i <= 3; ++i)
            SAMPLE_LOG_ERROR(K_SHUTDOWN_LOG_CATEGORY, "queued {}", i);
        {
            std::unique_lock<std::mutex> lock(g_gate.mutex);
            g_gate.changed.wait(lock, []()
            {
                return g_gate.entered;
            });
            g_gate.released = release;
        }
        g_gate.changed.notify_all();
        std::exit(0);
    }

#ifndef _WIN32
    /** @brief Arms the probe and exits; the probe logs after the logger stopped. */
    [[noreturn]] void exit_and_log_afterwards()
    {
        SushiLog::set_log_synchronous(false);
        SAMPLE_LOG_ERROR(K_SHUTDOWN_LOG_CATEGORY, "before exit");
        g_exit_probe.arm();
        std::exit(0);
    }
#endif
} // namespace

TEST(Small_LogExit, CallsAreValidDuringStaticInitialisation)
{
    EXPECT_EQ(g_static_level, LogLevel::Info);
}

#ifdef _WIN32

TEST(Medium_LogExit, AThreadEndedInsideTheSinkLosesTheQueue)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    // Windows ends the logger's thread inside the closed gate before SushiLog's statics die.
    EXPECT_EXIT(exit_with_records_queued(false), testing::ExitedWithCode(0), "^$");
}

TEST(Medium_LogExit, ACallAfterTheLoggerStoppedIsDelivered)
{
    GTEST_SKIP() << "On Windows a program's static destructors run before those of the DLLs "
                    "it loads, so no call of this program can follow the logger's shut-down.";
}

#else

TEST(Medium_LogExit, StaticDestructionDeliversWhatIsQueued)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    EXPECT_EXIT(exit_with_records_queued(true), testing::ExitedWithCode(0),
                "sink: queued 1\nsink: queued 2\nsink: queued 3");
}

TEST(Medium_LogExit, ACallAfterTheLoggerStoppedIsDelivered)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    EXPECT_EXIT(exit_and_log_afterwards(), testing::ExitedWithCode(0),
                "logged during exit");
}

#endif
