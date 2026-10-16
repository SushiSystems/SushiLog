/**************************************************************************/
/* consumer.cpp                                                           */
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
 * @file consumer.cpp
 * @brief Logs through an installed SushiLog target and fails unless the record arrives whole.
 * @author Mustafa Garip
 * @see tests/package/README.md
 */

#include <SushiLog/SushiLog.hpp>
#include <SushiLog/log/log_c_sink.h>

#include <cstdio>
#include <string>

namespace
{
    constexpr SushiLog::LogProject K_CONSUMER_PROJECT{"consumer", "CONSUMER_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_PACKAGE_LOG_CATEGORY =
        SushiLog::make_log_category(K_CONSUMER_PROJECT, "package");

    /** @brief Holds the message of the last record the C sink received. */
    std::string g_c_message;

    /** @brief Keeps the message of a record, as a host outside C++ would. */
    void keep_message(const SushiLogRecord* record, void*)
    {
        g_c_message.assign(record->message, record->message_size);
    }
} // namespace

/**
 * @brief Logs one record through the C++ sink and one through the C sink.
 * @return 0 when both arrived with their message, 1 otherwise.
 */
int main()
{
    std::string message;
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink([&message](const SushiLog::LogRecord& record)
    {
        message = std::string(record.message);
    }));
    SL_LOG_FRONT_AT(SushiLog::log_level_ceiling(true), SushiLog::LogLevel::Warn,
                    K_PACKAGE_LOG_CATEGORY, "through the {} target", CONSUMER_TARGET);

    sl_set_log_sink(&keep_message, nullptr);
    SL_LOG_FRONT_AT(SushiLog::log_level_ceiling(true), SushiLog::LogLevel::Error,
                    K_PACKAGE_LOG_CATEGORY, "through the C sink");
    sl_set_log_sink(nullptr, nullptr);

    const std::string expected = std::string("through the ") + CONSUMER_TARGET + " target";
    if (message != expected || g_c_message != "through the C sink")
    {
        std::fprintf(stderr, "consumer (%s): got '%s' and '%s'\n", CONSUMER_TARGET,
                     message.c_str(), g_c_message.c_str());
        return 1;
    }
    std::fprintf(stdout, "consumer (%s): both records arrived\n", CONSUMER_TARGET);
    return 0;
}
