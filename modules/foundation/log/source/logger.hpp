/**************************************************************************/
/* logger.hpp                                                             */
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
 * @file logger.hpp
 * @brief Declares Logger, which composes the level table, the queue, the sink and the writer.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include "level_environment.hpp"
#include "level_table.hpp"
#include "log_queue.hpp"
#include "log_writer.hpp"
#include "sink_holder.hpp"

namespace SushiLog::Detail
{
    /**
     * @brief Holds the one logger of the process; it composes the bricks and adds nothing.
     */
    struct Logger
    {
        LevelTable levels{&read_level_environment}; /**< The level of every category. */
        LogQueue queue{K_LOG_QUEUE_CAPACITY};       /**< The records waiting for the thread. */
        SinkHolder sink;                            /**< The sink every record reaches. */
        LogWriter writer{queue, sink};              /**< The thread and the delivery mode. */
    };

    /**
     * @brief Returns the logger of the process, made on the first call and never destroyed.
     *
     * Its thread stops, and what it queued is delivered, when the library's statics die.
     */
    [[nodiscard]] Logger& logger();
} // namespace SushiLog::Detail
