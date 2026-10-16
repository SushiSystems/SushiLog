/**************************************************************************/
/* level_environment.cpp                                                  */
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
 * @file level_environment.cpp
 * @brief Defines the reading of a project's level variable, such as SR_LOG_LEVEL.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "level_environment.hpp"

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <utility>

#include "level_name.hpp"
#include "stderr_line.hpp"

namespace SushiLog::Detail
{
    namespace
    {
        /** @brief Holds the level of a category that nothing sets. */
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

        /**
         * @brief Returns the value of the environment variable @p name, or nothing when it is
         *        unset or empty.
         */
        std::optional<std::string> environment_value(const std::string& name)
        {
            #ifdef _WIN32
                char* value = nullptr;
                std::size_t size = 0;
                if (_dupenv_s(&value, &size, name.c_str()) != 0 || value == nullptr)
                    return std::nullopt;
                std::string copy(value);
                std::free(value);
            #else
                const char* value = std::getenv(name.c_str());
                if (value == nullptr)
                    return std::nullopt;
                std::string copy(value);
            #endif
            if (copy.empty())
                return std::nullopt;
            return copy;
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
                const std::optional<LogLevel> level = level_named(item);
                if (!level)
                    return std::nullopt;
                setting.level = *level;
            }
            else
            {
                const std::string_view module = trimmed(item.substr(0, equals));
                const std::optional<LogLevel> level =
                    level_named(trimmed(item.substr(equals + 1)));
                if (module.empty() || !level)
                    return std::nullopt;
                setting.modules.push_back(ModuleLevel{to_lower(module), *level});
            }
            if (comma == std::string_view::npos)
                return setting;
            rest.remove_prefix(comma + 1);
        }
    }

    LevelSetting read_level_environment(LogProject project)
    {
        const std::string variable(project.level_variable());
        const std::optional<std::string> value = environment_value(variable);
        if (!value)
            return LevelSetting{K_DEFAULT_LOG_LEVEL, {}};

        std::optional<LevelSetting> setting = parse_level_setting(*value);
        if (setting)
            return std::move(*setting);

        write_stderr_line("[SushiLog] " + variable + "='" + *value +
                          "' is not a level or a list of module=level pairs (trace, debug, "
                          "info, warn, error, fatal, off); every level of " +
                          std::string(project.name()) + " stays warn.");
        return LevelSetting{K_DEFAULT_LOG_LEVEL, {}};
    }
} // namespace SushiLog::Detail
