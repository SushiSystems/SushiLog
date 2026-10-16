/**************************************************************************/
/* level_table.cpp                                                        */
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
 * @file level_table.cpp
 * @brief Defines LevelTable, the level of every project and category this logger has seen.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "level_table.hpp"

#include <cstddef>

namespace SushiLog::Detail
{
    LevelTable::LevelTable(LevelReader reader) noexcept : reader_(reader)
    {

    }

    std::atomic<int>& LevelTable::slot(LogCategory category)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return find_or_add(category).level;
    }

    void LevelTable::set_project(LogProject project, LogLevel level)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const std::size_t index = find_or_add_project(project);
        projects_[index].setting = LevelSetting{level, {}};
        for (Slot& entry : slots_)
        {
            if (entry.project == index && !entry.pinned)
                entry.level.store(static_cast<int>(level), std::memory_order_relaxed);
        }
    }

    LogLevel LevelTable::project_level(LogProject project)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return projects_[find_or_add_project(project)].setting.level;
    }

    void LevelTable::set_category(LogCategory category, LogLevel level)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        Slot& entry = find_or_add(category);
        entry.pinned = true;
        entry.level.store(static_cast<int>(level), std::memory_order_relaxed);
    }

    std::optional<std::size_t> LevelTable::find_project(LogProject project) const
    {
        for (std::size_t i = 0; i < projects_.size(); ++i)
        {
            if (projects_[i].name == project.name())
                return i;
        }
        return std::nullopt;
    }

    std::size_t LevelTable::find_or_add_project(LogProject project)
    {
        if (const std::optional<std::size_t> found = find_project(project))
            return *found;
        projects_.push_back(Project{std::string(project.name()), reader_(project)});
        return projects_.size() - 1;
    }

    LevelTable::Slot& LevelTable::find_or_add(LogCategory category)
    {
        const std::size_t index = find_or_add_project(category.project());

        for (Slot& entry : slots_)
        {
            if (entry.project == index && entry.module == category.module())
                return entry;
        }

        Slot& entry = slots_.emplace_back();
        entry.project = index;
        entry.module.assign(category.module().data(), category.module().size());
        entry.level.store(static_cast<int>(projects_[index].setting.level_of(entry.module)),
                          std::memory_order_relaxed);
        return entry;
    }
} // namespace SushiLog::Detail
