/**************************************************************************/
/* log_writer.cpp                                                         */
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
 * @file log_writer.cpp
 * @brief Defines LogWriter, which chooses when and on which thread a record is delivered.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "log_writer.hpp"

#include <cstdio>
#include <deque>
#include <exception>
#include <string>
#include <system_error>
#include <utility>

#include <SushiLog/log/log_level.hpp>

#include "stderr_line.hpp"

namespace SushiLog::Detail
{
    LogWriter::LogWriter(LogQueue& queue, SinkHolder& sink) noexcept : queue_(queue), sink_(sink)
    {

    }

    void LogWriter::write(QueuedRecord&& record)
    {
        if (record.level == LogLevel::Fatal)
        {
            write_fatal(record);
            return;
        }

        std::size_t dropped = 0;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!synchronous_ && !stopping_ && is_worker_running())
            {
                if (queue_.push(std::move(record)))
                    work_ready_.notify_one();
                return;
            }
            dropped = queue_.take_dropped();
        }
        deliver_after_report(dropped, record);
    }

    void LogWriter::set_synchronous(bool synchronous)
    {
        if (SinkHolder::is_delivering())
        {
            write_stderr_line("[SushiLog] a sink set the delivery mode from inside its own "
                              "call; the mode is unchanged.");
            return;
        }

        std::unique_lock<std::mutex> lock(mutex_);
        synchronous_ = synchronous;
        if (!synchronous)
            return;

        idle_.wait(lock, [this]()
        {
            return stopping_ || (queue_.is_empty() && !delivering_);
        });
    }

    void LogWriter::shut_down()
    {
        std::thread worker;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
            worker = std::move(worker_);
        }
        work_ready_.notify_all();
        idle_.notify_all();

        if (worker.joinable())
            worker.join();

        std::deque<QueuedRecord> remaining;
        std::size_t dropped = 0;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            remaining = queue_.take_all();
            dropped = queue_.take_dropped();
        }

        if (sink_.abandon_if_busy())
            return;

        for (const QueuedRecord& record : remaining)
        {
            deliver_after_report(dropped, record);
            dropped = 0;
        }
        if (dropped > 0)
        {
            try
            {
                sink_.deliver(make_drop_report(dropped, queue_.capacity()));
            }
            catch (const std::exception& error)
            {
                write_stderr_line(std::string("[SushiLog] records were dropped, and the report "
                                              "of it failed: ") + error.what());
            }
        }
    }

    void LogWriter::write_fatal(const QueuedRecord& record)
    {
        std::size_t dropped = 0;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            // From inside the sink the thread is this one, and waiting for it would never end.
            if (!SinkHolder::is_delivering())
            {
                idle_.wait(lock, [this]()
                {
                    return stopping_ || !worker_.joinable() ||
                           (queue_.is_empty() && !delivering_);
                });
            }
            dropped = queue_.take_dropped();
        }
        deliver_after_report(dropped, record);
        std::fflush(nullptr);
    }

    void LogWriter::deliver_after_report(std::size_t dropped, const QueuedRecord& record) noexcept
    {
        if (dropped > 0)
        {
            try
            {
                sink_.deliver(make_drop_report(dropped, queue_.capacity()));
            }
            catch (const std::exception& error)
            {
                write_stderr_line("[SushiLog] records were dropped, and the report of it "
                                  "failed; the record after them follows.");
                write_stderr_line(error.what());
            }
        }
        sink_.deliver(record);
    }

    bool LogWriter::is_worker_running()
    {
        if (worker_.joinable())
            return true;

        try
        {
            worker_ = std::thread(&LogWriter::run, this);
            return true;
        }
        catch (const std::system_error& error)
        {
            write_stderr_line(std::string("[SushiLog] the log thread did not start (") +
                              error.what() + "); records are delivered on the calling thread.");
            synchronous_ = true;
            return false;
        }
    }

    void LogWriter::run()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (true)
        {
            work_ready_.wait(lock, [this]()
            {
                return stopping_ || !queue_.is_empty();
            });

            if (queue_.is_empty())
                return;

            QueuedRecord record = queue_.pop();
            const std::size_t dropped = queue_.take_dropped();
            delivering_ = true;
            lock.unlock();

            deliver_after_report(dropped, record);

            lock.lock();
            delivering_ = false;
            if (queue_.is_empty())
                idle_.notify_all();
        }
    }
} // namespace SushiLog::Detail
