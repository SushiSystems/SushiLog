/**************************************************************************/
/* log_record.hpp                                                         */
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
 * @file log_record.hpp
 * @brief Declares LogRecord, the fields of one record a sink receives.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <cstdint>
#include <string_view>

#include <SushiLog/log/log_level.hpp>

namespace SushiLog
{
    /**
     * @brief Holds one record, each field apart; every view is valid only during the sink call.
     */
    struct LogRecord
    {
        LogLevel level;           /**< The level the record was logged at. */
        std::string_view project; /**< The project of the record's category. */
        std::string_view module;  /**< The module of the record's category. */
        std::string_view file;    /**< The file of the call, without its folders. */
        std::uint32_t line;       /**< The line of the call. */
        std::uint32_t thread;     /**< The sending logger's number for the calling thread. */
        std::int64_t time_ns;     /**< System clock time of the call, ns since the Unix epoch. */
        std::string_view message; /**< The formatted message alone. */
        std::string_view text;    /**< The whole line of the record. */
    };
} // namespace SushiLog
