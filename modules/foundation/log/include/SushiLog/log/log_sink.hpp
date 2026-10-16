/**************************************************************************/
/* log_sink.hpp                                                           */
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
 * @file log_sink.hpp
 * @brief Declares LogSink, the callable that receives each record.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <SushiLog/log/delegate.hpp>
#include <SushiLog/log/log_record.hpp>

namespace SushiLog
{
    /**
     * @brief Receives one record. The record is valid only during the call; a sink does not
     *        log and does not set the sink, a level or the delivery mode.
     */
    using LogSink = Delegate<void(const LogRecord& record), 48>;
} // namespace SushiLog
