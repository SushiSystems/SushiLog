/**************************************************************************/
/* sink_holder.cpp                                                        */
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
 * @file sink_holder.cpp
 * @brief Defines SinkHolder, which holds the sink, swaps it and delivers one record to it.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "sink_holder.hpp"

#include <exception>
#include <string>
#include <utility>

#include "stderr_line.hpp"

namespace SushiLog::Detail
{
    namespace
    {
        /// @brief True while the calling thread is inside a call into the sink.
        thread_local bool t_delivering = false;

        /** @brief Sets t_delivering for the life of one sink call. */
        class DeliveryScope final
        {
            public:
                DeliveryScope() noexcept
                {
                    t_delivering = true;
                }

                DeliveryScope(const DeliveryScope&) = delete;
                DeliveryScope& operator=(const DeliveryScope&) = delete;
                DeliveryScope(DeliveryScope&&) = delete;
                DeliveryScope& operator=(DeliveryScope&&) = delete;

                ~DeliveryScope()
                {
                    t_delivering = false;
                }
        };

        /** @brief Writes the reason a record went to stderr, then the record's line. */
        void write_fallback(std::string_view reason, const QueuedRecord& record) noexcept
        {
            write_stderr_line(reason);
            write_stderr_line(record.text);
        }
    } // namespace

    bool SinkHolder::is_delivering() noexcept
    {
        return t_delivering;
    }

    void SinkHolder::deliver(const QueuedRecord& record) noexcept
    {
        if (t_delivering)
        {
            write_fallback("[SushiLog] a sink logged from inside its own call; its record "
                           "follows on stderr.", record);
            return;
        }
        // Once set, abandoned_ stays set and mutex_ is never released.
        if (abandoned_.load(std::memory_order_acquire))
        {
            write_stderr_line(record.text);
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        if (!sink_)
        {
            write_stderr_line(record.text);
            return;
        }

        try
        {
            const DeliveryScope scope;
            sink_(record.view());
        }
        catch (const std::exception& error)
        {
            try
            {
                write_fallback("[SushiLog] the log sink threw '" + std::string(error.what()) +
                               "'; its record follows on stderr.", record);
            }
            catch (const std::exception&)
            {
                write_fallback("[SushiLog] the log sink threw; its record follows on stderr.",
                               record);
            }
        }
        catch (...)
        {
            write_fallback("[SushiLog] the log sink threw a value that is not a "
                           "std::exception; its record follows on stderr.", record);
        }
    }

    bool SinkHolder::replace(LogSink sink)
    {
        if (t_delivering)
        {
            write_stderr_line("[SushiLog] a sink set the sink from inside its own call; the "
                              "sink is unchanged.");
            return false;
        }
        // Once set, abandoned_ stays set and mutex_ is never released.
        if (abandoned_.load(std::memory_order_acquire))
            return false;

        // The old sink leaves in the parameter and dies after the unlock.
        std::lock_guard<std::mutex> lock(mutex_);
        std::swap(sink_, sink);
        return true;
    }

    bool SinkHolder::abandon_if_busy() noexcept
    {
        if (!mutex_.try_lock())
        {
            abandoned_.store(true, std::memory_order_release);
            return true;
        }
        mutex_.unlock();
        return false;
    }
} // namespace SushiLog::Detail
