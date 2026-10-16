/**************************************************************************/
/* log_abi.hpp                                                            */
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
 * @file log_abi.hpp
 * @brief Declares K_ABI_VERSION, abi_version() and the check a front runs on its first call.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include <SushiLog/log/export.hpp>
#include <SushiLog/log/log_project.hpp>

namespace SushiLog
{
    /**
     * @brief Holds the version of SushiLog's types and calls a front is compiled against.
     *
     * It grows on every change to a type or a call of the interface, and never otherwise.
     */
    inline constexpr std::uint32_t K_ABI_VERSION = 1;

    /** @brief Returns the K_ABI_VERSION the loaded library was compiled with. */
    [[nodiscard]] SUSHILOG_API std::uint32_t abi_version() noexcept;

    namespace Detail
    {
        /**
         * @brief Writes one stderr line naming @p project, both versions and the library's
         *        path, then terminates the process.
         *
         * Its parameters are C types, so it can be called across libraries of two versions.
         */
        [[noreturn]] SUSHILOG_API void report_abi_mismatch(const char* project,
                                                           std::size_t project_size,
                                                           std::uint32_t front_version,
                                                           std::uint32_t library_version) noexcept;

        /**
         * @brief Calls report_abi_mismatch() when @p front_version and @p library_version
         *        differ; a front passes K_ABI_VERSION and abi_version().
         */
        inline void check_log_abi(LogProject project, std::uint32_t front_version,
                                  std::uint32_t library_version) noexcept
        {
            if (front_version != library_version)
                report_abi_mismatch(project.name().data(), project.name().size(), front_version,
                                    library_version);
        }
    } // namespace Detail
} // namespace SushiLog
