# SushiLogTiers.cmake
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

# The module order of the repository, stated once. tools/settings.py holds the checkers' copy.

# Lowest tier first.
set(SUSHILOG_TIER_ORDER
    core
    foundation)

# Module, then its tier. A dependency is listed before its consumer.
set(SUSHILOG_MODULE_TIERS
    text        core
    log         foundation
    umbrella    foundation)

# Sets <out> to the tier of <module>, or to an empty string for a module the table lacks.
function(sushilog_module_tier module out)
    set(result "")
    list(LENGTH SUSHILOG_MODULE_TIERS length)
    math(EXPR last "${length} - 2")
    foreach(position RANGE 0 ${last} 2)
        list(GET SUSHILOG_MODULE_TIERS ${position} name)
        if(name STREQUAL "${module}")
            math(EXPR tier_position "${position} + 1")
            list(GET SUSHILOG_MODULE_TIERS ${tier_position} result)
            break()
        endif()
    endforeach()
    set(${out} "${result}" PARENT_SCOPE)
endfunction()

# Sets <out> to "<tier>/<module>" for every module, in table order.
function(sushilog_module_folders out)
    set(folders "")
    list(LENGTH SUSHILOG_MODULE_TIERS length)
    math(EXPR last "${length} - 2")
    foreach(position RANGE 0 ${last} 2)
        math(EXPR tier_position "${position} + 1")
        list(GET SUSHILOG_MODULE_TIERS ${position} name)
        list(GET SUSHILOG_MODULE_TIERS ${tier_position} tier)
        list(APPEND folders "${tier}/${name}")
    endforeach()
    set(${out} "${folders}" PARENT_SCOPE)
endfunction()
