/**************************************************************************/
/* log_front.cpp                                                          */
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
 * @file log_front.cpp
 * @brief Defines the two calls the front macro makes into the library.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include <SushiLog/log/log_front.hpp>

#include <cstdint>
#include <exception>
#include <utility>

#include "logger.hpp"
#include "record_format.hpp"
#include "stderr_line.hpp"

namespace SushiLog::Detail
{
    std::atomic<int>& log_level_slot(LogCategory category) noexcept
    {
        try
        {
            return logger().levels.slot(category);
        }
        catch (const std::exception& error)
        {
            // Passes Warn and above, the default, for a category the table could not add.
            static std::atomic<int> fallback{static_cast<int>(LogLevel::Warn)};
            write_stderr_line(std::string("[SushiLog] a log category could not be added (") +
                              error.what() + "); it logs at warn.");
            return fallback;
        }
    }

    void emit_log_record(LogLevel level, LogCategory category, const char* file, int line,
                         std::string&& message)
    {
        logger().writer.write(make_record(level, category.project().name(), category.module(),
                                          file, static_cast<std::uint32_t>(line), now_ns(),
                                          std::move(message)));
    }
} // namespace SushiLog::Detail
