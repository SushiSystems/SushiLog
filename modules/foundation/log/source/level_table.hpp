/**************************************************************************/
/* level_table.hpp                                                        */
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
 * @file level_table.hpp
 * @brief Declares LevelTable, the level of every project and category this logger has seen.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <atomic>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <SushiLog/log/log_category.hpp>
#include <SushiLog/log/log_level.hpp>
#include <SushiLog/log/log_project.hpp>

#include "level_environment.hpp"

namespace SushiLog::Detail
{
    /** @brief Returns what a project's level variable sets; called once per project. */
    using LevelReader = LevelSetting (*)(LogProject project);

    /**
     * @brief Holds one atomic level per category and the level of each project.
     *
     * A project is added on the first use of one of its categories, when @c reader reads its
     * variable. A category's atomic keeps its address for the life of the process.
     */
    class LevelTable final
    {
        public:
            /** @brief Holds @p reader, which gives each project its levels when it is added. */
            explicit LevelTable(LevelReader reader) noexcept;

            LevelTable(const LevelTable&) = delete;
            LevelTable& operator=(const LevelTable&) = delete;
            LevelTable(LevelTable&&) = delete;
            LevelTable& operator=(LevelTable&&) = delete;
            ~LevelTable() = default;

            /** @brief Returns the atomic level of @p category, adding it on first use. */
            [[nodiscard]] std::atomic<int>& slot(LogCategory category);

            /**
             * @brief Sets @p project and every category of it set_category() has not pinned.
             * @return False, changing nothing, when no category of @p project was added.
             */
            [[nodiscard]] bool set_project(LogProject project, LogLevel level);

            /**
             * @brief Returns the level of @p project's categories that nothing pins.
             * @return Nothing when no category of @p project was added.
             */
            [[nodiscard]] std::optional<LogLevel> project_level(LogProject project);

            /** @brief Sets one category, adding it first, and pins it against set_project(). */
            void set_category(LogCategory category, LogLevel level);

        private:
            /** @brief Holds one project and what its variable said. */
            struct Project
            {
                std::string name;     /**< The project name. */
                LevelSetting setting; /**< Its level and its module=level pairs. */
            };

            /** @brief Holds the level of one category. */
            struct Slot
            {
                std::size_t project = 0;   /**< The index of its project. */
                std::string module;        /**< The module. */
                std::atomic<int> level{0}; /**< Its level, as an int. */
                bool pinned = false;       /**< True once set_category() named it. */
            };

            /**
             * @brief Returns the index of @p project, or nothing when it was never added.
             * @pre mutex_ is held.
             */
            [[nodiscard]] std::optional<std::size_t> find_project(LogProject project) const;

            /**
             * @brief Returns the slot of @p category, adding it and its project on first use.
             * @pre mutex_ is held.
             */
            [[nodiscard]] Slot& find_or_add(LogCategory category);

            /// @brief Reads a project's variable when the project is added.
            LevelReader reader_;
            /// @brief Guards everything but the values of the atomics.
            std::mutex mutex_;
            /// @brief The projects, in the order of their first use.
            std::vector<Project> projects_;
            /// @brief The categories; a deque keeps each slot's address as it grows.
            std::deque<Slot> slots_;
    };
} // namespace SushiLog::Detail
