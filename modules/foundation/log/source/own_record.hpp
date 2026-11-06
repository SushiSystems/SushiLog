/**************************************************************************/
/* own_record.hpp                                                         */
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
 * @file own_record.hpp
 * @brief Declares make_own_record, which writes a record the logger makes about itself.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <cstdint>
#include <string>

#include <SushiLog/log/log_category.hpp>
#include <SushiLog/log/log_level.hpp>
#include <SushiLog/log/log_project.hpp>

#include "record_format.hpp"

namespace SushiLog::Detail
{
    /** @brief Names SushiLog's own records, whose levels SL_LOG_LEVEL sets. */
    inline constexpr LogProject K_SUSHILOG_LOG_PROJECT{"sushilog", "SL_LOG_LEVEL"};

    /** @brief Names the category of the records the logger writes about itself. */
    inline constexpr LogCategory K_LOG_LOG_CATEGORY =
        make_log_category(K_SUSHILOG_LOG_PROJECT, "log");

    /**
     * @brief Returns a record of K_LOG_LOG_CATEGORY at @p level, made now on this thread.
     *
     * The logger writes it past every level gate: it reports what the logger did to a record.
     */
    [[nodiscard]] QueuedRecord make_own_record(LogLevel level, const char* file,
                                               std::uint32_t line, std::string&& message);
} // namespace SushiLog::Detail
