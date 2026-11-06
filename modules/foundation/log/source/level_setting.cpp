/**************************************************************************/
/* level_setting.cpp                                                      */
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
 * @file level_setting.cpp
 * @brief Defines parse_level_setting, the reading of a level variable's value.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include <SushiLog/log/level_setting.hpp>

#include <cctype>
#include <cstddef>

#include "level_name.hpp"

namespace SushiLog
{
    namespace
    {
        /** @brief Holds the level of every module a value names no level for. */
        constexpr LogLevel K_DEFAULT_LOG_LEVEL = LogLevel::Warn;

        /** @brief Returns @p text without its leading and trailing spaces. */
        std::string_view trimmed(std::string_view text) noexcept
        {
            while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())))
                text.remove_prefix(1);
            while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back())))
                text.remove_suffix(1);
            return text;
        }

        /** @brief Returns @p text in lower case. */
        std::string to_lower(std::string_view text)
        {
            std::string out(text);
            for (char& c : out)
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            return out;
        }
    } // namespace

    LogLevel LevelSetting::level_of(std::string_view module) const noexcept
    {
        LogLevel found = level;
        for (const ModuleLevel& item : modules)
        {
            if (item.module == module)
                found = item.level;
        }
        return found;
    }

    std::optional<LevelSetting> parse_level_setting(std::string_view value)
    {
        LevelSetting setting{K_DEFAULT_LOG_LEVEL, {}};
        std::string_view rest = value;
        while (true)
        {
            const std::size_t comma = rest.find(',');
            const std::string_view item = trimmed(rest.substr(0, comma));
            const std::size_t equals = item.find('=');
            if (equals == std::string_view::npos)
            {
                const std::optional<LogLevel> level = Detail::level_named(item);
                if (!level)
                    return std::nullopt;
                setting.level = *level;
            }
            else
            {
                const std::string_view module = trimmed(item.substr(0, equals));
                const std::optional<LogLevel> level =
                    Detail::level_named(trimmed(item.substr(equals + 1)));
                if (module.empty() || !level)
                    return std::nullopt;
                setting.modules.push_back(ModuleLevel{to_lower(module), *level});
            }
            if (comma == std::string_view::npos)
                return setting;
            rest.remove_prefix(comma + 1);
        }
    }
} // namespace SushiLog
