/**************************************************************************/
/* thread_numbers.hpp                                                     */
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
 * @file thread_numbers.hpp
 * @brief Defines ThreadNumbers, which gives each identity a number in the order first seen.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace SushiLog
{
    /**
     * @brief Gives each identity it is shown a number, 1 for the first, counting up in the
     *        order identities are first seen; an identity keeps its number. Safe from any thread.
     * @tparam Identity A copyable identity with equality, such as std::thread::id.
     * @tparam Hash The hash of @p Identity.
     */
    template <class Identity, class Hash = std::hash<Identity>>
    class ThreadNumbers final
    {
        public:
            /** @brief Returns the number of @p identity, giving it the next one on first sight. */
            [[nodiscard]] std::uint32_t number_of(const Identity& identity)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                const auto [entry, added] = numbers_.try_emplace(identity, next_);
                if (added)
                    ++next_;
                return entry->second;
            }

        private:
            /// @brief Guards numbers_ and next_.
            std::mutex mutex_;
            /// @brief The number of each identity seen.
            std::unordered_map<Identity, std::uint32_t, Hash> numbers_;
            /// @brief The number the next new identity gets.
            std::uint32_t next_ = 1;
    };
} // namespace SushiLog
