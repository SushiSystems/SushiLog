/**************************************************************************/
/* stderr_line.cpp                                                        */
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
 * @file stderr_line.cpp
 * @brief Defines write_stderr_line, the one place the logger writes to the console.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "stderr_line.hpp"

#include <cstdio>

namespace SushiLog::Detail
{
    void write_stderr_line(std::string_view line) noexcept
    {
        std::fprintf(stderr, "%.*s\n", static_cast<int>(line.size()), line.data());
        std::fflush(stderr);
    }
} // namespace SushiLog::Detail
