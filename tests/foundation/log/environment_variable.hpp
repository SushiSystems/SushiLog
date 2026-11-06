/**************************************************************************/
/* environment_variable.hpp                                               */
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
 * @file environment_variable.hpp
 * @brief Defines ScopedEnvironmentVariable, which sets a variable and puts the old value back.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#pragma once

#include <cstdlib>
#include <optional>
#include <string>
#include <utility>

namespace SushiLogTests
{
    /** @brief Sets the environment variable @p name of this process to @p value. */
    inline void set_environment_variable(const std::string& name, const std::string& value)
    {
        #ifdef _WIN32
            _putenv_s(name.c_str(), value.c_str());
        #else
            setenv(name.c_str(), value.c_str(), 1);
        #endif
    }

    /** @brief Removes the environment variable @p name from this process. */
    inline void unset_environment_variable(const std::string& name)
    {
        #ifdef _WIN32
            _putenv_s(name.c_str(), "");
        #else
            unsetenv(name.c_str());
        #endif
    }

    /**
     * @brief Returns the value of the environment variable @p name, or nothing when it is unset.
     */
    inline std::optional<std::string> read_environment_variable(const std::string& name)
    {
        #ifdef _WIN32
            char* buffer = nullptr;
            std::size_t length = 0;
            if (_dupenv_s(&buffer, &length, name.c_str()) != 0 || buffer == nullptr)
                return std::nullopt;
            std::string value(buffer);
            std::free(buffer);
            return value;
        #else
            const char* value = std::getenv(name.c_str());
            if (value == nullptr)
                return std::nullopt;
            return std::string(value);
        #endif
    }

    /**
     * @brief Sets an environment variable for the life of the object and puts back the value
     *        it found, or unsets it when it found none.
     */
    class ScopedEnvironmentVariable final
    {
        public:
            /** @brief Sets @p name to @p value, keeping what @p name held. */
            ScopedEnvironmentVariable(std::string name, const std::string& value)
                : name_(std::move(name)), previous_(read_environment_variable(name_))
            {
                set_environment_variable(name_, value);
            }

            ScopedEnvironmentVariable(const ScopedEnvironmentVariable&) = delete;
            ScopedEnvironmentVariable& operator=(const ScopedEnvironmentVariable&) = delete;
            ScopedEnvironmentVariable(ScopedEnvironmentVariable&&) = delete;
            ScopedEnvironmentVariable& operator=(ScopedEnvironmentVariable&&) = delete;

            ~ScopedEnvironmentVariable()
            {
                if (previous_.has_value())
                    set_environment_variable(name_, *previous_);
                else
                    unset_environment_variable(name_);
            }

        private:
            /// @brief The variable this object set.
            std::string name_;
            /// @brief The value found on entry, empty when the variable was unset.
            std::optional<std::string> previous_;
    };
} // namespace SushiLogTests
