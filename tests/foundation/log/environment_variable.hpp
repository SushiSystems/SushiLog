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
 * @brief Defines set_environment_variable, which the level-variable tests set a value through.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#pragma once

#include <cstdlib>
#include <string>

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
} // namespace SushiLogTests
