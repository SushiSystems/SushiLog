# SushiLogBuildPolicy.cmake
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

# The compile policy of this tree: warnings and the sanitizer SL_SANITIZE names. Both reach the
# targets of this tree only; an installed consumer inherits neither.

add_library(sushilog_warnings INTERFACE)
if(MSVC)
    target_compile_options(sushilog_warnings INTERFACE /W4 /WX /permissive- /EHsc)
    target_compile_definitions(sushilog_warnings INTERFACE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(sushilog_warnings INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

add_library(sushilog_sanitizers INTERFACE)
if(SL_SANITIZE STREQUAL "address")
    if(MSVC)
        target_compile_options(sushilog_sanitizers INTERFACE /fsanitize=address)
    else()
        target_compile_options(sushilog_sanitizers INTERFACE
            -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
        target_link_options(sushilog_sanitizers INTERFACE -fsanitize=address,undefined)
    endif()
elseif(SL_SANITIZE STREQUAL "thread")
    if(MSVC)
        message(FATAL_ERROR "SL_SANITIZE=thread: MSVC has no ThreadSanitizer; use clang or gcc.")
    endif()
    target_compile_options(sushilog_sanitizers INTERFACE -fsanitize=thread)
    target_link_options(sushilog_sanitizers INTERFACE -fsanitize=thread)
elseif(NOT SL_SANITIZE STREQUAL "")
    message(FATAL_ERROR "SL_SANITIZE='${SL_SANITIZE}': use address, thread or nothing.")
endif()

# Applies the warnings and the sanitizer to one target of this tree. BUILD_INTERFACE keeps
# both names out of the exported link interface.
function(sushilog_apply_build_policy target)
    target_link_libraries(${target} PRIVATE
        "$<BUILD_INTERFACE:sushilog_warnings>"
        "$<BUILD_INTERFACE:sushilog_sanitizers>")
endfunction()
