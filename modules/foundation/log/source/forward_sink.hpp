/**************************************************************************/
/* forward_sink.hpp                                                       */
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
 * @file forward_sink.hpp
 * @brief Declares forwarded_record, which turns another logger's record into one of this logger.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <SushiLog/log/log_record.hpp>

#include "record_format.hpp"

namespace SushiLog::Detail
{
    /**
     * @brief Returns @p record as this logger queues it: its level, category, call site, time
     *        and message kept, the calling thread numbered here and the line written here.
     *
     * The sender's thread and text are ignored, and no level gate applies.
     */
    [[nodiscard]] QueuedRecord forwarded_record(const LogRecord& record);
} // namespace SushiLog::Detail
