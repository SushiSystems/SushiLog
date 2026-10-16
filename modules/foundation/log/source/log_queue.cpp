/**************************************************************************/
/* log_queue.cpp                                                          */
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
 * @file log_queue.cpp
 * @brief Defines LogQueue, the bounded queue of records and the count of those it dropped.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "log_queue.hpp"

#include <string>
#include <utility>

#include "own_record.hpp"

namespace SushiLog::Detail
{
    LogQueue::LogQueue(std::size_t capacity) noexcept : capacity_(capacity)
    {

    }

    bool LogQueue::push(QueuedRecord&& record)
    {
        if (records_.size() >= capacity_)
        {
            ++dropped_;
            return false;
        }
        records_.push_back(std::move(record));
        return true;
    }

    bool LogQueue::is_empty() const noexcept
    {
        return records_.empty();
    }

    QueuedRecord LogQueue::pop()
    {
        QueuedRecord record = std::move(records_.front());
        records_.pop_front();
        return record;
    }

    std::deque<QueuedRecord> LogQueue::take_all() noexcept
    {
        std::deque<QueuedRecord> taken;
        taken.swap(records_);
        return taken;
    }

    std::size_t LogQueue::take_dropped() noexcept
    {
        return std::exchange(dropped_, 0);
    }

    std::size_t LogQueue::capacity() const noexcept
    {
        return capacity_;
    }

    QueuedRecord make_drop_report(std::size_t dropped, std::size_t capacity)
    {
        return make_own_record(LogLevel::Warn, __FILE__, __LINE__,
                               "dropped " + std::to_string(dropped) +
                                   " records: the log queue of " + std::to_string(capacity) +
                                   " was full; the records after them are delivered");
    }
} // namespace SushiLog::Detail
