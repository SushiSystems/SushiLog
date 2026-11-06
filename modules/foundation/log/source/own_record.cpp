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
