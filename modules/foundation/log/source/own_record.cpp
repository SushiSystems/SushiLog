/**************************************************************************/
/* own_record.cpp                                                         */
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
 * @file own_record.cpp
 * @brief Defines make_own_record, which writes a record the logger makes about itself.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "own_record.hpp"

#include <utility>

namespace SushiLog::Detail
{
    QueuedRecord make_own_record(LogLevel level, const char* file, std::uint32_t line,
                                 std::string&& message)
    {
        return make_record(level, K_LOG_LOG_CATEGORY.project().name(),
                           K_LOG_LOG_CATEGORY.module(), file, line, now_ns(),
                           std::move(message));
    }
} // namespace SushiLog::Detail
