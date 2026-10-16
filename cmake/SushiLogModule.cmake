# SushiLogModule.cmake
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

# One function declares a module. A module is an INTERFACE target carrying its include root
# and those of the modules it depends on; its sources are recorded for the library targets that
# cmake/SushiLogLibrary.cmake composes, and each of its public headers is compiled alone.

include("${CMAKE_CURRENT_LIST_DIR}/SushiLogTiers.cmake")

# Refuses a dependency the tier table does not allow. An umbrella module includes its scope.
function(sushilog_check_module_edge consumer dependency)
    sushilog_module_tier("${dependency}" dependency_tier)
    if(dependency_tier STREQUAL "")
        message(FATAL_ERROR
            "sushilog_add_module(${consumer}): depends on '${dependency}', which "
            "SUSHILOG_MODULE_TIERS (cmake/SushiLogTiers.cmake) does not list.")
    endif()
    if(NOT TARGET sushilog_module_${dependency})
        message(FATAL_ERROR
            "sushilog_add_module(${consumer}): '${dependency}' is not declared yet; a "
            "dependency comes before its consumer in SUSHILOG_MODULE_TIERS.")
    endif()
    if(consumer STREQUAL "umbrella")
        return()
    endif()
    sushilog_module_tier("${consumer}" consumer_tier)
    list(FIND SUSHILOG_TIER_ORDER "${consumer_tier}" consumer_index)
    list(FIND SUSHILOG_TIER_ORDER "${dependency_tier}" dependency_index)
    if(dependency_index GREATER_EQUAL consumer_index)
        message(FATAL_ERROR
            "sushilog_add_module(${consumer}): ${dependency} (${dependency_tier}) is not below "
            "${consumer} (${consumer_tier}) in SUSHILOG_TIER_ORDER.")
    endif()
endfunction()

# Compiles every public header of a module alone, with its declared include roots only, so a
# header that forgot one of its own includes fails here.
function(sushilog_add_header_check module include_root)
    file(GLOB_RECURSE headers CONFIGURE_DEPENDS RELATIVE "${include_root}"
        "${include_root}/*.hpp" "${include_root}/*.h")
    set(units "")
    foreach(header IN LISTS headers)
        string(MAKE_C_IDENTIFIER "${header}" unit_name)
        set(unit "${CMAKE_CURRENT_BINARY_DIR}/header_check/${unit_name}.cpp")
        file(CONFIGURE OUTPUT "${unit}" CONTENT "#include <${header}>\n" @ONLY)
        list(APPEND units "${unit}")
    endforeach()
    if(NOT units)
        return()
    endif()
    set(check "sushilog_module_${module}_header_check")
    add_library(${check} OBJECT ${units})
    target_link_libraries(${check} PRIVATE "sushilog_module_${module}")
    sushilog_apply_build_policy(${check})
endfunction()

# sushilog_add_module(NAME <module> TIER <tier> [HEADER_ONLY]
#                     [SOURCES <file under source/>...] [DEPENDS <module>...])
function(sushilog_add_module)
    cmake_parse_arguments(ARG "HEADER_ONLY" "NAME;TIER" "SOURCES;DEPENDS" ${ARGN})
    if("${ARG_NAME}" STREQUAL "" OR "${ARG_TIER}" STREQUAL "")
        message(FATAL_ERROR "sushilog_add_module(): NAME and TIER are required.")
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "sushilog_add_module(${ARG_NAME}): unexpected '${ARG_UNPARSED_ARGUMENTS}'.")
    endif()
    sushilog_module_tier("${ARG_NAME}" table_tier)
    if(NOT "${ARG_TIER}" STREQUAL "${table_tier}")
        message(FATAL_ERROR
            "sushilog_add_module(${ARG_NAME}): declares TIER '${ARG_TIER}'; "
            "cmake/SushiLogTiers.cmake says '${table_tier}'.")
    endif()
    if(ARG_HEADER_ONLY AND ARG_SOURCES)
        message(FATAL_ERROR
            "sushilog_add_module(${ARG_NAME}): HEADER_ONLY and SOURCES exclude each other.")
    endif()
    if(NOT ARG_HEADER_ONLY AND NOT ARG_SOURCES)
        message(FATAL_ERROR "sushilog_add_module(${ARG_NAME}): no SOURCES; mark it HEADER_ONLY.")
    endif()

    set(target "sushilog_module_${ARG_NAME}")
    set(include_root "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_library(${target} INTERFACE)
    target_include_directories(${target} INTERFACE "$<BUILD_INTERFACE:${include_root}>")
    foreach(dependency IN LISTS ARG_DEPENDS)
        sushilog_check_module_edge("${ARG_NAME}" "${dependency}")
        target_link_libraries(${target} INTERFACE "sushilog_module_${dependency}")
    endforeach()
    set_property(GLOBAL APPEND PROPERTY SUSHILOG_INCLUDE_ROOTS "${include_root}")

    foreach(source IN LISTS ARG_SOURCES)
        set_property(GLOBAL APPEND PROPERTY SUSHILOG_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/source/${source}")
    endforeach()
    if(ARG_SOURCES)
        set_property(GLOBAL APPEND PROPERTY SUSHILOG_SOURCE_ROOTS
            "${CMAKE_CURRENT_SOURCE_DIR}/source")
    endif()

    sushilog_add_header_check("${ARG_NAME}" "${include_root}")
endfunction()
