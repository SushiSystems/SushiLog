/**************************************************************************/
/* log_abi.cpp                                                            */
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
 * @file log_abi.cpp
 * @brief Defines abi_version() and the report of a front compiled against another version.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include <SushiLog/log/log_abi.hpp>

#include <exception>
#include <string>

#include "library_path.hpp"
#include "stderr_line.hpp"

namespace SushiLog
{
    std::uint32_t abi_version() noexcept
    {
        return K_ABI_VERSION;
    }

    namespace Detail
    {
        void report_abi_mismatch(const char* project, std::size_t project_size,
                                 std::uint32_t front_version,
                                 std::uint32_t library_version) noexcept
        {
            try
            {
                write_stderr_line("[SushiLog] " + std::string(project, project_size) +
                                  " was compiled against SushiLog ABI " +
                                  std::to_string(front_version) + ", and the loaded " +
                                  library_path() + " has ABI " +
                                  std::to_string(library_version) +
                                  "; rebuild " + std::string(project, project_size) +
                                  " against the SushiLog it loads. The process ends.");
            }
            catch (const std::exception& error)
            {
                write_stderr_line("[SushiLog] a front was compiled against another SushiLog ABI "
                                  "than the loaded library's, and naming them failed. The "
                                  "process ends.");
                write_stderr_line(error.what());
            }
            std::terminate();
        }
    } // namespace Detail
} // namespace SushiLog
