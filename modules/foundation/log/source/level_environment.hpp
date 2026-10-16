/**************************************************************************/
/* level_environment.hpp                                                  */
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
 * @file level_environment.hpp
 * @brief Declares the reading of a project's level variable, such as SR_LOG_LEVEL.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <SushiLog/log/log_level.hpp>
#include <SushiLog/log/log_project.hpp>

namespace SushiLog::Detail
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
        [[nodiscard]] LogLevel level_of(std::string_view module) const noexcept;
    };

    /**
     * @brief Reads @p value: a level for every module, then any module=level pairs, any case.
     * @return Nothing when an item is neither a level nor a module=level pair.
     */
    [[nodiscard]] std::optional<LevelSetting> parse_level_setting(std::string_view value);

    /**
     * @brief Reads @p project's level variable once and returns what it sets.
     *
     * An unset or empty variable sets Warn. A value parse_level_setting() cannot read is
     * reported on stderr and sets Warn for every module of the project.
     */
    [[nodiscard]] LevelSetting read_level_environment(LogProject project);
} // namespace SushiLog::Detail
