# SushiLogRuntimeFiles.cmake
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

# Copies the shared libraries an executable loads beside it, so it starts without a search path.

# sushilog_copy_runtime_files(<target>)
#
# On Windows, copies every DLL in $<TARGET_RUNTIME_DLLS:target> into the target's folder after
# each link. Elsewhere the build tree's RPATH finds the shared library, and nothing is copied.
function(sushilog_copy_runtime_files target)
    if(NOT WIN32)
        return()
    endif()
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${target}> $<TARGET_FILE_DIR:${target}>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()
