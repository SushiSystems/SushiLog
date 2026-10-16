/**************************************************************************/
/* library_path.hpp                                                       */
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
 * @file library_path.hpp
 * @brief Declares library_path, the file the SushiLog code of this process was loaded from.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <string>

namespace SushiLog::Detail
{
    /**
     * @brief Returns the path of the shared library or executable that holds this function.
     * @return "(unknown)" when the system does not say.
     */
    [[nodiscard]] std::string library_path();
} // namespace SushiLog::Detail
