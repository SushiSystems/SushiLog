/**************************************************************************/
/* log_project.hpp                                                        */
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
 * @file log_project.hpp
 * @brief Declares LogProject, a project that logs and the variable that sets its levels.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <string_view>

namespace SushiLog
{
    /**
     * @brief Names a project that logs and the environment variable that sets its levels.
     *
     * Both views refer to string literals, so a project lives for the whole process.
     */
    class LogProject final
    {
        public:
            /**
             * @brief Holds @p name, the project in lower case, and @p level_variable, the
             *        variable its levels are read from, such as SR_LOG_LEVEL.
             */
            constexpr LogProject(std::string_view name, std::string_view level_variable) noexcept
                : name_(name), level_variable_(level_variable)
            {

            }

            /** @brief Returns the name a record shows before the module. */
            [[nodiscard]] constexpr std::string_view name() const noexcept
            {
                return name_;
            }

            /** @brief Returns the environment variable that sets the project's levels. */
            [[nodiscard]] constexpr std::string_view level_variable() const noexcept
            {
                return level_variable_;
            }

        private:
            /// @brief The project name, in lower case.
            std::string_view name_;
            /// @brief The environment variable read on the first use of the project.
            std::string_view level_variable_;
    };
} // namespace SushiLog
