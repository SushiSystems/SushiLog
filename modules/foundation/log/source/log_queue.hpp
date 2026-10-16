/**************************************************************************/
/* log_queue.hpp                                                          */
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
 * @file log_queue.hpp
 * @brief Declares LogQueue, the bounded queue of records and the count of those it dropped.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <cstddef>
#include <deque>

#include "record_format.hpp"

namespace SushiLog::Detail
{
    /** @brief Holds how many records wait for the log thread before one is dropped. */
    inline constexpr std::size_t K_LOG_QUEUE_CAPACITY = 4096;

    /**
     * @brief Holds the records waiting for the log thread, oldest first, up to a capacity.
     *
     * Not synchronised; its owner guards every call with one mutex.
     */
    class LogQueue final
    {
        public:
            /** @brief Holds at most @p capacity records. */
            explicit LogQueue(std::size_t capacity) noexcept;

            LogQueue(const LogQueue&) = delete;
            LogQueue& operator=(const LogQueue&) = delete;
            LogQueue(LogQueue&&) = delete;
            LogQueue& operator=(LogQueue&&) = delete;
            ~LogQueue() = default;

            /**
             * @brief Appends @p record, or counts it as dropped when the queue is full.
             * @return False when the record was dropped.
             */
            [[nodiscard]] bool push(QueuedRecord&& record);

            /** @brief Returns whether no record waits. */
            [[nodiscard]] bool is_empty() const noexcept;

            /**
             * @brief Removes and returns the oldest record.
             * @pre is_empty() is false.
             */
            [[nodiscard]] QueuedRecord pop();

            /** @brief Removes and returns every waiting record, oldest first. */
            [[nodiscard]] std::deque<QueuedRecord> take_all() noexcept;

            /** @brief Returns how many records were dropped since the last call, and resets it. */
            [[nodiscard]] std::size_t take_dropped() noexcept;

            /** @brief Returns how many records the queue holds at most. */
            [[nodiscard]] std::size_t capacity() const noexcept;

        private:
            /// @brief The most records the queue holds.
            std::size_t capacity_;
            /// @brief The waiting records, oldest first.
            std::deque<QueuedRecord> records_;
            /// @brief The records dropped since take_dropped() last ran.
            std::size_t dropped_ = 0;
    };

    /**
     * @brief Returns the WARN delivered before the next record, naming how many the full
     *        queue of @p capacity dropped.
     * @pre @p dropped is above zero.
     */
    [[nodiscard]] QueuedRecord make_drop_report(std::size_t dropped, std::size_t capacity);
} // namespace SushiLog::Detail
