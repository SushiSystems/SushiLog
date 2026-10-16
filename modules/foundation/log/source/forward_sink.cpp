/**************************************************************************/
/* forward_sink.cpp                                                       */
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
 * @file forward_sink.cpp
 * @brief Defines forwarded_record, which turns another logger's record into one of this logger.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "forward_sink.hpp"

#include <string>

namespace SushiLog::Detail
{
    QueuedRecord forwarded_record(const LogRecord& record)
    {
        return make_record(record.level, record.project, record.module, record.file,
                           record.line, record.time_ns, std::string(record.message));
    }
} // namespace SushiLog::Detail
