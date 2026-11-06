/**************************************************************************/
/* level_name.hpp                                                         */
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
 * @file level_name.hpp
 * @brief Declares the names of the levels, as a record writes them and a variable reads them.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <optional>
#include <string_view>

#include <SushiLog/log/log_level.hpp>

namespace SushiLog::Detail
{
    /** @brief Returns the label a record of @p level carries, such as WARN. */
    [[nodiscard]] std::string_view level_label(LogLevel level) noexcept;

    /**
     * @brief Returns the level @p text names, ignoring case.
     * @return Nothing when @p text names no level.
     */
    [[nodiscard]] std::optional<LogLevel> level_named(std::string_view text) noexcept;
} // namespace SushiLog::Detail
