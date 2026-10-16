/**************************************************************************/
/* stderr_line.hpp                                                        */
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
 * @file stderr_line.hpp
 * @brief Declares write_stderr_line, the one place the logger writes to the console.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <string_view>

namespace SushiLog::Detail
{
    /** @brief Writes @p line and a line break to stderr in one call, then flushes stderr. */
    void write_stderr_line(std::string_view line) noexcept;
} // namespace SushiLog::Detail
