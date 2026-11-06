/**************************************************************************/
/* compile_delegate.cpp                                                   */
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
 * @file compile_delegate.cpp
 * @brief Checks at compile time which callables fit a Delegate's storage.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <type_traits>

#include <SushiLog/log/delegate.hpp>

namespace
{
    /** @brief A callable that holds 8 bytes. */
    struct SmallCall
    {
        long long held; /**< Its state. */

        /** @brief Does nothing. */
        void operator()() const
        {
        }
    };

    /** @brief A callable that holds 32 bytes. */
    struct LargeCall
    {
        long long held[4]; /**< Its state. */

        /** @brief Does nothing. */
        void operator()() const
        {
        }
    };

    /** @brief A Delegate of 16 bytes. */
    using SixteenBytes = SushiLog::Delegate<void(), 16>;
} // namespace

static_assert(SixteenBytes::fits<SmallCall>);
static_assert(!SixteenBytes::fits<LargeCall>);
static_assert(std::is_constructible_v<SixteenBytes, SmallCall>);
static_assert(!std::is_constructible_v<SixteenBytes, LargeCall>);
