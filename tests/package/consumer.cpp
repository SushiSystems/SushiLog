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
 * @file consumer.cpp
 * @brief Logs through an installed SushiLog target and exits nonzero unless the record arrives whole.
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

    /** @brief Puts back the default sink and the delivery mode it found when it ends. */
    class DeliveryGuard final
    {
        public:
            DeliveryGuard() : was_synchronous_(SushiLog::log_synchronous())
            {

            }

            DeliveryGuard(const DeliveryGuard&) = delete;
            DeliveryGuard& operator=(const DeliveryGuard&) = delete;

            ~DeliveryGuard()
            {
                sl_set_log_sink(nullptr, nullptr);
                SushiLog::set_log_sink(SushiLog::LogSink());
                SushiLog::set_log_synchronous(was_synchronous_);
            }

        private:
            bool was_synchronous_;
    };
} // namespace

/**
 * @brief Logs one record through the C++ sink and one through the C sink, and exits with the
 *        status of a comparison of what arrived with what was sent.
 * @return 0 when both messages arrived whole, 1 otherwise.
 */
int main()
{
    const DeliveryGuard guard;
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

    const std::string expected = std::string("through the ") + CONSUMER_TARGET + " target";
    const bool arrived = message == expected && g_c_message == "through the C sink";
    std::fprintf(stdout, "consumer (%s): got '%s' and '%s'\n", CONSUMER_TARGET, message.c_str(),
                 g_c_message.c_str());
    return static_cast<int>(!arrived);
}
