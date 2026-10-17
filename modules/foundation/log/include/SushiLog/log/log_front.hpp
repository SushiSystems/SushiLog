/**************************************************************************/
/* log_front.hpp                                                          */
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
 * @file log_front.hpp
 * @brief Declares SL_LOG_FRONT_AT and log_level_ceiling, on which a project defines its macros.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <atomic>
#include <string>
#include <string_view>

#include <SushiLog/log/export.hpp>
#include <SushiLog/log/log_abi.hpp>
#include <SushiLog/log/log_category.hpp>
#include <SushiLog/text/text_format.hpp>
#include <SushiLog/log/log_level.hpp>

namespace SushiLog
{
    /**
     * @brief Returns the most detailed level a translation unit compiles log calls for.
     *
     * Error when @p enabled is false, Debug under NDEBUG, Trace otherwise. It has internal
     * linkage, so each translation unit reads its own NDEBUG.
     */
    [[nodiscard]] [[maybe_unused]] static constexpr LogLevel log_level_ceiling(
        bool enabled) noexcept
    {
        if (!enabled)
            return LogLevel::Error;
        #ifdef NDEBUG
            return LogLevel::Debug;
        #else
            return LogLevel::Trace;
        #endif
    }

    namespace Detail
    {
        /**
         * @brief Returns the atomic level of @p category, reading its project's variable on the
         *        first use of the project.
         *
         * A call site keeps the reference in a static, so each later call costs one relaxed
         * load. The reference stays valid for the life of the process.
         */
        [[nodiscard]] SUSHILOG_API std::atomic<int>& log_level_slot(LogCategory category) noexcept;

        /**
         * @brief Checks the library's ABI version against this front's, then returns the
         *        atomic level of @p category; the front macro calls it once per call site.
         */
        [[nodiscard]] inline std::atomic<int>& front_level_slot(LogCategory category) noexcept
        {
            check_log_abi(category.project(), K_ABI_VERSION, abi_version());
            return log_level_slot(category);
        }

        /**
         * @brief Hands one formatted message to the logger, which stamps its time and thread.
         */
        SUSHILOG_API void emit_log_record(LogLevel level, LogCategory category, const char* file,
                                          int line, std::string&& message);

        /**
         * @brief Formats one record and hands it to the logger; the front macro calls it past
         *        both gates.
         */
        template <typename... Args>
        void log_record(LogLevel level, LogCategory category, const char* file, int line,
                        std::string_view format, const Args&... args)
        {
            emit_log_record(level, category, file, line, format_text(format, args...));
        }
    } // namespace Detail
} // namespace SushiLog

/**
 * @brief Logs one record at @p level in @p category when @p ceiling and the category's level
 *        both admit it.
 *
 * A project defines its macros on it, passing its own ceiling. A call past @p ceiling compiles
 * to nothing; one below the category's level costs one relaxed load and a branch.
 */
#define SL_LOG_FRONT_AT(ceiling, level, category, ...)                                           \
    do                                                                                           \
    {                                                                                            \
        if constexpr ((level) >= (ceiling))                                                      \
        {                                                                                        \
            static std::atomic<int>& sl_log_level_slot_ =                                        \
                SushiLog::Detail::front_level_slot(category);                                    \
            if (static_cast<int>(level) >= sl_log_level_slot_.load(std::memory_order_relaxed))   \
            {                                                                                    \
                SushiLog::Detail::log_record((level), (category), __FILE__, __LINE__,            \
                                             __VA_ARGS__);                                       \
            }                                                                                    \
        }                                                                                        \
    } while (0)
