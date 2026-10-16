/**************************************************************************/
/* level_name.cpp                                                         */
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
 * @file level_name.cpp
 * @brief Defines the names of the levels, as a record writes them and a variable reads them.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "level_name.hpp"

#include <cctype>
#include <cstddef>

namespace SushiLog::Detail
{
    namespace
    {
        /** @brief Pairs a level with the label of its records. */
        struct LevelName
        {
            LogLevel level;         /**< The level. */
            std::string_view label; /**< What a record calls it. */
        };

        /** @brief Lists every level with its label. */
        constexpr LevelName K_LEVEL_NAMES[] = {
            {LogLevel::Trace, "TRACE"},
            {LogLevel::Debug, "DEBUG"},
            {LogLevel::Info, "INFO"},
            {LogLevel::Warn, "WARN"},
            {LogLevel::Error, "ERROR"},
            {LogLevel::Fatal, "FATAL"},
            {LogLevel::Off, "OFF"},
        };

        /** @brief Returns whether @p text equals @p name when the case of both is ignored. */
        bool equals_ignoring_case(std::string_view text, std::string_view name) noexcept
        {
            if (text.size() != name.size())
                return false;
            for (std::size_t i = 0; i < text.size(); ++i)
            {
                if (std::tolower(static_cast<unsigned char>(text[i])) !=
                    std::tolower(static_cast<unsigned char>(name[i])))
                    return false;
            }
            return true;
        }
    } // namespace

    std::string_view level_label(LogLevel level) noexcept
    {
        for (const LevelName& entry : K_LEVEL_NAMES)
        {
            if (entry.level == level)
                return entry.label;
        }
        return "LOG";
    }

    std::optional<LogLevel> level_named(std::string_view text) noexcept
    {
        for (const LevelName& entry : K_LEVEL_NAMES)
        {
            if (equals_ignoring_case(text, entry.label))
                return entry.level;
        }
        return std::nullopt;
    }
} // namespace SushiLog::Detail
