/**************************************************************************/
/* delivery_mode_guard.hpp                                                */
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
 * @file delivery_mode_guard.hpp
 * @brief Defines SinkGuard and DeliveryModeGuard, which put back what a test changed.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#pragma once

#include <utility>

#include <SushiLog/log/log_calls.hpp>

namespace SushiLogTests
{
    /**
     * @brief Installs a sink for the life of the object and puts back the sink it found.
     *
     * Declare it before a DeliveryModeGuard, so the queue is empty when the old sink returns.
     */
    class SinkGuard final
    {
        public:
            /** @brief Installs @p sink, keeping the sink in place. */
            explicit SinkGuard(SushiLog::LogSink sink)
                : previous_(std::move(sink))
            {
                installed_ = SushiLog::exchange_log_sink(previous_);
            }

            SinkGuard(const SinkGuard&) = delete;
            SinkGuard& operator=(const SinkGuard&) = delete;
            SinkGuard(SinkGuard&&) = delete;
            SinkGuard& operator=(SinkGuard&&) = delete;

            ~SinkGuard()
            {
                if (installed_)
                    static_cast<void>(SushiLog::exchange_log_sink(previous_));
            }

        private:
            /// @brief Whether the new sink went in, so the exit has one to swap out.
            bool installed_ = false;
            /// @brief The sink found on entry, put back on exit.
            SushiLog::LogSink previous_;
    };

    /**
     * @brief Sets a delivery mode for the life of the object, waits for the queue to empty on
     *        exit and puts back the mode it found.
     */
    class DeliveryModeGuard final
    {
        public:
            /** @brief Sets synchronous delivery when @p synchronous is true, else asynchronous. */
            explicit DeliveryModeGuard(bool synchronous)
                : was_synchronous_(SushiLog::log_synchronous())
            {
                SushiLog::set_log_synchronous(synchronous);
            }

            DeliveryModeGuard(const DeliveryModeGuard&) = delete;
            DeliveryModeGuard& operator=(const DeliveryModeGuard&) = delete;
            DeliveryModeGuard(DeliveryModeGuard&&) = delete;
            DeliveryModeGuard& operator=(DeliveryModeGuard&&) = delete;

            ~DeliveryModeGuard()
            {
                SushiLog::set_log_synchronous(true);
                SushiLog::set_log_synchronous(was_synchronous_);
            }

        private:
            /// @brief The delivery mode found on entry.
            bool was_synchronous_;
    };
} // namespace SushiLogTests
