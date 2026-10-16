/**************************************************************************/
/* test_forward.cpp                                                       */
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
 * @file test_forward.cpp
 * @brief Tests log_forward_sink: no gate, the sender's time kept, its thread and text replaced.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_calls.hpp>

#include <string>

#include <gtest/gtest.h>

#include "recording_sink.hpp"

namespace
{
    constexpr SushiLog::LogProject K_HOST_PROJECT{"hosttest", "HOSTTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_EDITOR_LOG_CATEGORY =
        SushiLog::make_log_category(K_HOST_PROJECT, "editor");

    /** @brief Returns a record another logger made, with a thread and text of its own. */
    SushiLog::LogRecord foreign_record()
    {
        return SushiLog::LogRecord{SushiLog::LogLevel::Debug, "hosttest", "editor",
                                   "editor.cpp", 31, 777, 987654321, "scene saved",
                                   "a line written by the sender"};
    }
} // namespace

TEST(Small_LogForward, AForwardedRecordPassesNoLevelGate)
{
    const SushiLogTests::SynchronousRecording recording;
    SushiLog::set_log_level(K_EDITOR_LOG_CATEGORY, SushiLog::LogLevel::Off);
    const SushiLog::LogSink forward = SushiLog::log_forward_sink();

    forward(foreign_record());

    ASSERT_EQ(recording.sink().lines().size(), 1u);
    EXPECT_EQ(recording.sink().lines()[0].level, SushiLog::LogLevel::Debug);
}

TEST(Small_LogForward, AForwardedRecordKeepsItsTimeAndCallSite)
{
    const SushiLogTests::SynchronousRecording recording;
    const SushiLog::LogSink forward = SushiLog::log_forward_sink();

    forward(foreign_record());

    ASSERT_EQ(recording.sink().lines().size(), 1u);
    const SushiLogTests::RecordedLine line = recording.sink().lines()[0];
    EXPECT_EQ(line.time_ns, 987654321);
    EXPECT_EQ(line.file, "editor.cpp");
    EXPECT_EQ(line.line, 31u);
    EXPECT_EQ(line.message, "scene saved");
}

TEST(Small_LogForward, AForwardedRecordGetsThisLoggersThreadAndLine)
{
    const SushiLogTests::SynchronousRecording recording;
    const SushiLog::LogSink forward = SushiLog::log_forward_sink();

    forward(foreign_record());

    ASSERT_EQ(recording.sink().lines().size(), 1u);
    const SushiLogTests::RecordedLine line = recording.sink().lines()[0];
    EXPECT_NE(line.thread, 777u);
    EXPECT_EQ(line.text.find("a line written by the sender"), std::string::npos);
    EXPECT_NE(line.text.find("[DEBUG] [hosttest.editor]"), std::string::npos);
    EXPECT_NE(line.text.find("[editor.cpp:31] scene saved"), std::string::npos);
}
