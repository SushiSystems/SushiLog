/**************************************************************************/
/* level_setting.hpp                                                      */
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
 * @file level_setting.hpp
 * @brief Declares LevelSetting and parse_level_setting, the reading of a level variable's value.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <SushiLog/log/export.hpp>
#include <SushiLog/log/log_level.hpp>

namespace SushiLog
{
    /** @brief Holds the level a project's variable names for one of its modules. */
    struct ModuleLevel
    {
        std::string module; /**< The module, in lower case. */
        LogLevel level;     /**< Its level. */
    };

    /** @brief Holds what one project's level variable says. */
    struct LevelSetting
    {
        LogLevel level;                   /**< The level of every module not named. */
        std::vector<ModuleLevel> modules; /**< The module=level pairs, in order. */

        /** @brief Returns the level @p module takes: its last pair, else the project's level. */
        [[nodiscard]] SUSHILOG_API LogLevel level_of(std::string_view module) const noexcept;
    };

    /**
     * @brief Reads @p value: a level for every module, then any module=level pairs, any case,
     *        with spaces around each item. Touches no environment and writes nothing.
     * @return Nothing when an item is neither a level nor a module=level pair.
     */
    [[nodiscard]] SUSHILOG_API std::optional<LevelSetting> parse_level_setting(
        std::string_view value);
} // namespace SushiLog
