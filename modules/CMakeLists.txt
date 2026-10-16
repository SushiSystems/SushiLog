# CMakeLists.txt
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

# Adds every module folder in the order cmake/SushiLogTiers.cmake states.

sushilog_module_folders(folders)
foreach(folder IN LISTS folders)
    add_subdirectory(${folder})
endforeach()
