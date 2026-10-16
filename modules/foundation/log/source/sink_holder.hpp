/**************************************************************************/
/* sink_holder.hpp                                                        */
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
 * @file sink_holder.hpp
 * @brief Declares SinkHolder, which holds the sink, swaps it and delivers one record to it.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <atomic>
#include <mutex>

#include <SushiLog/log/log_sink.hpp>

#include "record_format.hpp"

namespace SushiLog::Detail
{
    /**
     * @brief Holds the sink of the process and is the only code that calls it.
     *
     * An empty sink means stderr. A sink that throws, a record that re-enters the sink from
     * inside it, and a sink abandoned by a thread the system ended all fall back to stderr.
     */
    class SinkHolder final
    {
        public:
            SinkHolder() = default;
            SinkHolder(const SinkHolder&) = delete;
            SinkHolder& operator=(const SinkHolder&) = delete;
            SinkHolder(SinkHolder&&) = delete;
            SinkHolder& operator=(SinkHolder&&) = delete;
            ~SinkHolder() = default;

            /** @brief Returns whether the calling thread is inside a call into the sink. */
            [[nodiscard]] static bool is_delivering() noexcept;

            /**
             * @brief Calls the sink with @p record, or writes its line to stderr when no sink is
             *        set, the sink throws, the call re-enters from inside the sink, or the sink
             *        was abandoned.
             */
            void deliver(const QueuedRecord& record) noexcept;

            /**
             * @brief Swaps @p sink in once no call into the old one runs.
             * @return False, changing nothing, when called from inside the sink or after the
             *         sink was abandoned.
             */
            [[nodiscard]] bool replace(LogSink sink);

            /**
             * @brief Marks the sink abandoned when its mutex is held at shut-down, which happens
             *        only when the system ended a thread inside a sink call.
             * @return True when the sink was abandoned; every later record goes to stderr.
             */
            [[nodiscard]] bool abandon_if_busy() noexcept;

        private:
            /// @brief Guards sink_ and is held across every call into it.
            std::mutex mutex_;
            /// @brief The sink of the process; empty means stderr.
            LogSink sink_;
            /// @brief Set once when abandon_if_busy() finds mutex_ held; never cleared.
            std::atomic<bool> abandoned_{false};
    };
} // namespace SushiLog::Detail
