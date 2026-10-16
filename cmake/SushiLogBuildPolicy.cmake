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

# Applies the warnings and the sanitizer to one target of this tree. Their settings are copied
# onto the target rather than linked, because a static library carries even a private link into
# its link interface, and a build-tree export would then require both names in its export set.
function(sushilog_apply_build_policy target)
    get_target_property(type ${target} TYPE)
    foreach(policy IN ITEMS sushilog_warnings sushilog_sanitizers)
        target_compile_options(${target} PRIVATE
            "$<TARGET_PROPERTY:${policy},INTERFACE_COMPILE_OPTIONS>")
        target_compile_definitions(${target} PRIVATE
            "$<TARGET_PROPERTY:${policy},INTERFACE_COMPILE_DEFINITIONS>")
        if(NOT type STREQUAL "STATIC_LIBRARY")
            target_link_options(${target} PRIVATE
                "$<TARGET_PROPERTY:${policy},INTERFACE_LINK_OPTIONS>")
        endif()
    endforeach()
endfunction()
