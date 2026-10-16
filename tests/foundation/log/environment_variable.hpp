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
