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
 * @file level_environment.cpp
 * @brief Defines the reading of a project's level variable, such as SR_LOG_LEVEL.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "level_environment.hpp"

#include <cstddef>
#include <cstdlib>
#include <optional>
#include <string>
#include <utility>

#include "stderr_line.hpp"

namespace SushiLog::Detail
{
    namespace
    {
        /** @brief Holds the level of a category that nothing sets. */
        constexpr LogLevel K_DEFAULT_LOG_LEVEL = LogLevel::Warn;

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
