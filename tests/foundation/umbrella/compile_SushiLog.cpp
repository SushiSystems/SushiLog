/**************************************************************************/
/* compile_SushiLog.cpp                                                   */
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
 * @file compile_SushiLog.cpp
 * @brief Checks at compile time that the umbrella header names the levels, the sink and the front, and leaves the C header out.
 * @author Mustafa Garip
 * @see tests/foundation/umbrella/README.md
 */

#include <SushiLog/SushiLog.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

/**
 * @brief Holds a type of the C header's name; it compiles only while the umbrella leaves
 *        log_c_sink.h out.
 */
struct SushiLogRecord
{
    int declared_here; /**< A field the C header's struct does not have. */
};

namespace
{
    constexpr SushiLog::LogProject K_UMBRELLA_PROJECT{"umbrellatest", "UMBRELLATEST_LOG_LEVEL"};
    constexpr SushiLog::LogCategory K_USE_LOG_CATEGORY =
        SushiLog::make_log_category(K_UMBRELLA_PROJECT, "use");

    static_assert(std::is_enum_v<SushiLog::LogLevel>);
    static_assert(std::is_class_v<SushiLog::LogRecord>);
    static_assert(std::is_constructible_v<SushiLog::LogSink, std::nullptr_t>);
    static_assert(std::is_invocable_v<decltype(&SushiLog::set_log_sink), SushiLog::LogSink>);
    static_assert(std::is_same_v<std::remove_const_t<decltype(SushiLog::K_ABI_VERSION)>,
                                 std::uint32_t>);

    /** @brief Expands the front macro, so the compiler checks that the umbrella names all it needs. */
    [[maybe_unused]] void log_through_the_front()
    {
        SL_LOG_FRONT_AT(SushiLog::log_level_ceiling(true), SushiLog::LogLevel::Error,
                        K_USE_LOG_CATEGORY, "reached {}", 1);
    }
} // namespace
