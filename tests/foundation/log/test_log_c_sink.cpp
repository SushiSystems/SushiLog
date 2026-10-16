/**************************************************************************/
/* test_log_c_sink.cpp                                                    */
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
 * @file test_log_c_sink.cpp
 * @brief Tests the C interface: a C sink, the size rule and sl_forward_record.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_c_sink.h>

#include <cstddef>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <SushiLog/log/log_calls.hpp>

#include "recording_sink.hpp"
#include "sample_front.hpp"

namespace
{
    constexpr SushiLog::LogProject K_C_PROJECT{"ctest", "CTEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_BRIDGE_LOG_CATEGORY =
        SushiLog::make_log_category(K_C_PROJECT, "bridge");

    /** @brief Holds what the C sink below was handed, owning its strings. */
    struct CRecordCopy
    {
        std::uint32_t size = 0;    /**< The record's size field. */
        std::uint8_t level = 0;    /**< Its level number. */
        std::string project;       /**< Its project. */
        std::string module;        /**< Its module. */
        std::string message;       /**< Its message. */
        std::string text;          /**< Its line. */
        void* user = nullptr;      /**< The user pointer it came with. */
    };

    /** @brief Keeps a copy of every record the C sink receives. */
    std::vector<CRecordCopy> g_c_records;

    /** @brief A SushiLogSinkFunction that copies the record into g_c_records. */
    void copy_c_record(const SushiLogRecord* record, void* user)
    {
        g_c_records.push_back(CRecordCopy{record->size, record->level,
                                          std::string(record->project, record->project_size),
                                          std::string(record->module, record->module_size),
                                          std::string(record->message, record->message_size),
                                          std::string(record->text, record->text_size), user});
    }

    /** @brief Returns a full C record of this version, as another logger would fill it. */
    SushiLogRecord foreign_record()
    {
        SushiLogRecord record{};
        record.size = sizeof(SushiLogRecord);
        record.level = 4;
        record.project = "hostproject";
        record.project_size = 11;
        record.module = "input";
        record.module_size = 5;
        record.file = "input.cpp";
        record.file_size = 9;
        record.line = 77;
        record.thread = 999;
        record.time_ns = 123456789;
        record.message = "key lost";
        record.message_size = 8;
        record.text = "a line this logger ignores";
        record.text_size = 26;
        return record;
    }
} // namespace

TEST(Small_LogCSink, ACSinkReceivesEveryFieldWithItsSize)
{
    g_c_records.clear();
    int marker = 0;
    sl_set_log_synchronous(1);
    sl_set_log_sink(&copy_c_record, &marker);

    SAMPLE_LOG_ERROR(K_BRIDGE_LOG_CATEGORY, "device {} lost", 2);
    sl_set_log_sink(nullptr, nullptr);

    ASSERT_EQ(g_c_records.size(), 1u);
    const CRecordCopy& record = g_c_records[0];
    EXPECT_EQ(record.size, sizeof(SushiLogRecord));
    EXPECT_EQ(record.level, 4u);
    EXPECT_EQ(record.project, "ctest");
    EXPECT_EQ(record.module, "bridge");
    EXPECT_EQ(record.message, "device 2 lost");
    EXPECT_NE(record.text.find("[ctest.bridge]"), std::string::npos);
    EXPECT_EQ(record.user, &marker);
}

TEST(Small_LogCSink, ForwardRecordKeepsTimeAndRenumbersTheThread)
{
    const SushiLogTests::SynchronousRecording recording;
    const SushiLogRecord record = foreign_record();

    sl_forward_record(&record, nullptr);

    ASSERT_EQ(recording.sink().lines().size(), 1u);
    const SushiLogTests::RecordedLine line = recording.sink().lines()[0];
    EXPECT_EQ(line.level, SushiLog::LogLevel::Error);
    EXPECT_EQ(line.project, "hostproject");
    EXPECT_EQ(line.module, "input");
    EXPECT_EQ(line.file, "input.cpp");
    EXPECT_EQ(line.line, 77u);
    EXPECT_EQ(line.time_ns, 123456789);
    EXPECT_EQ(line.message, "key lost");
    EXPECT_NE(line.thread, 999u);
    EXPECT_NE(line.text.find("[hostproject.input]"), std::string::npos);
}

TEST(Small_LogCSink, ForwardRecordReadsOnlyTheFieldsItsSizeCovers)
{
    const SushiLogTests::SynchronousRecording recording;
    SushiLogRecord record = foreign_record();
    record.size = static_cast<std::uint32_t>(offsetof(SushiLogRecord, thread));

    sl_forward_record(&record, nullptr);

    ASSERT_EQ(recording.sink().lines().size(), 1u);
    const SushiLogTests::RecordedLine line = recording.sink().lines()[0];
    EXPECT_EQ(line.line, 77u);
    EXPECT_EQ(line.time_ns, 0);
    EXPECT_EQ(line.message, "");
}

TEST(Medium_LogCSink, ANullFunctionRestoresStderr)
{
    g_c_records.clear();
    sl_set_log_synchronous(1);
    sl_set_log_sink(&copy_c_record, nullptr);
    sl_set_log_sink(nullptr, nullptr);

    testing::internal::CaptureStderr();
    SAMPLE_LOG_ERROR(K_BRIDGE_LOG_CATEGORY, "after the reset");
    const std::string err = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(g_c_records.empty());
    EXPECT_NE(err.find("after the reset"), std::string::npos);
}
