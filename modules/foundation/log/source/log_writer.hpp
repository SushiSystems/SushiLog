/**************************************************************************/
/* log_writer.hpp                                                         */
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
 * @file log_writer.hpp
 * @brief Declares LogWriter, which chooses when and on which thread a record is delivered.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>

#include "log_queue.hpp"
#include "record_format.hpp"
#include "sink_holder.hpp"

namespace SushiLog::Detail
{
    /**
     * @brief Runs the logger's thread over the queue, or delivers on the calling thread in
     *        synchronous mode, for FATAL and after shut_down().
     */
    class LogWriter final
    {
        public:
            /** @brief Delivers through @p sink the records it keeps in @p queue. */
            LogWriter(LogQueue& queue, SinkHolder& sink) noexcept;

            LogWriter(const LogWriter&) = delete;
            LogWriter& operator=(const LogWriter&) = delete;
            LogWriter(LogWriter&&) = delete;
            LogWriter& operator=(LogWriter&&) = delete;
            ~LogWriter() = default;

            /**
             * @brief Queues @p record for the thread, or delivers it on this thread in
             *        synchronous mode and after shut_down().
             *
             * A FATAL record is delivered on this thread after every queued one, then every
             * output stream is flushed.
             */
            void write(QueuedRecord&& record);

            /**
             * @brief Sets the delivery mode; entering synchronous mode waits for the queue.
             *
             * From inside the sink it changes nothing and says so on stderr.
             */
            void set_synchronous(bool synchronous);

            /** @brief Stops the thread and delivers what it left queued on this thread. */
            void shut_down();

        private:
            /** @brief Delivers @p record on this thread after the queue is empty, then flushes. */
            void write_fatal(const QueuedRecord& record);

            /** @brief Delivers the drop report of @p dropped records, then @p record. */
            void deliver_after_report(std::size_t dropped, const QueuedRecord& record) noexcept;

            /**
             * @brief Returns whether the thread exists, starting it on the first call.
             * @pre mutex_ is held.
             */
            [[nodiscard]] bool is_worker_running();

            /** @brief Delivers queued records until shut_down() finds the queue empty. */
            void run();

            /// @brief The records waiting for the thread.
            LogQueue& queue_;
            /// @brief The sink every record goes through.
            SinkHolder& sink_;
            /// @brief Guards queue_, worker_, synchronous_, stopping_ and delivering_.
            std::mutex mutex_;
            /// @brief Wakes the thread when a record arrives or shut_down() runs.
            std::condition_variable work_ready_;
            /// @brief Wakes a waiter when the queue is empty and delivered.
            std::condition_variable idle_;
            /// @brief The thread that delivers in asynchronous mode.
            std::thread worker_;
            /// @brief True while each log call delivers its own record.
            bool synchronous_ = false;
            /// @brief True from shut_down() on; no record is queued after it.
            bool stopping_ = false;
            /// @brief True while the thread is inside a delivery.
            bool delivering_ = false;
    };
} // namespace SushiLog::Detail
