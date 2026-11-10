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
 * @file log_writer.cpp
 * @brief Defines LogWriter, which chooses when and on which thread a record is delivered.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "log_writer.hpp"

#include <cstdio>
#include <deque>
#include <memory>
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
        bool queued = false;
        std::shared_ptr<const LogExecutor> poster;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!synchronous_ && !stopping_ && (executor_ || is_worker_running()))
            {
                queued = true;
                if (queue_.push(std::move(record)))
                {
                    if (executor_)
                        poster = claim_post();
                    else
                        work_ready_.notify_one();
                }
            }
            else
            {
                dropped = queue_.take_dropped();
            }
        }
        if (queued)
        {
            post(poster);
            return;
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

        if (executor_)
        {
            // No thread of ours empties the queue, so this one does.
            idle_.wait(lock, [this]()
            {
                return stopping_ || !delivering_;
            });
            drain_locked(lock);
            return;
        }

        idle_.wait(lock, [this]()
        {
            return stopping_ || (queue_.is_empty() && !delivering_);
        });
    }

    void LogWriter::set_executor(LogExecutor executor)
    {
        std::shared_ptr<const LogExecutor> poster;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            executor_ = executor ? std::make_shared<const LogExecutor>(std::move(executor))
                                 : nullptr;
            scheduled_ = false;
            if (executor_ && !queue_.is_empty())
                poster = claim_post();
            else if (!executor_ && !queue_.is_empty() && !synchronous_ && !stopping_)
                static_cast<void>(is_worker_running());
        }
        work_ready_.notify_all();
        post(poster);
    }

    bool LogWriter::is_synchronous()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return synchronous_;
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
            if (!SinkHolder::is_delivering() && executor_)
            {
                idle_.wait(lock, [this]()
                {
                    return !delivering_;
                });
                drain_locked(lock);
            }
            else if (!SinkHolder::is_delivering())
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

    void LogWriter::drain()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        scheduled_ = false;
        if (delivering_)
            return;
        drain_locked(lock);
    }

    void LogWriter::drain_locked(std::unique_lock<std::mutex>& lock)
    {
        while (!queue_.is_empty())
        {
            QueuedRecord record = queue_.pop();
            const std::size_t dropped = queue_.take_dropped();
            delivering_ = true;
            lock.unlock();

            deliver_after_report(dropped, record);

            lock.lock();
            delivering_ = false;
        }
        idle_.notify_all();
    }

    std::shared_ptr<const LogExecutor> LogWriter::claim_post()
    {
        if (!executor_ || scheduled_)
            return nullptr;
        scheduled_ = true;
        return executor_;
    }

    void LogWriter::post(const std::shared_ptr<const LogExecutor>& executor)
    {
        if (!executor)
            return;
        try
        {
            (*executor)(LogTask([this]()
            {
                drain();
            }));
        }
        catch (const std::exception& error)
        {
            write_stderr_line(std::string("[SushiLog] the log executor failed (") +
                              error.what() + "); the records are delivered on the calling "
                              "thread.");
            drain();
        }
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
                return stopping_ || (!executor_ && !queue_.is_empty());
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
