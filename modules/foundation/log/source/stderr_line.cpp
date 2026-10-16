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
