# SushiLogLibrary.cmake
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

# Composes the module sources into the two library targets.
#
#   sushilog         SushiLog::SushiLog, shared; one per process, whatever loads it.
#   sushilog_static  SushiLog::SushiLogStatic, static with hidden symbols, for a plug-in a host
#                    may load twice; it joins the host through the C interface.

# Declares one library target over the recorded sources.
function(sushilog_add_library_target target kind export_name)
    get_property(sources      GLOBAL PROPERTY SUSHILOG_SOURCES)
    get_property(include_roots GLOBAL PROPERTY SUSHILOG_INCLUDE_ROOTS)
    get_property(source_roots GLOBAL PROPERTY SUSHILOG_SOURCE_ROOTS)

    add_library(${target} ${kind} ${sources})
    add_library(SushiLog::${export_name} ALIAS ${target})
    foreach(include_root IN LISTS include_roots)
        target_include_directories(${target} PUBLIC "$<BUILD_INTERFACE:${include_root}>")
    endforeach()
    target_include_directories(${target} PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    target_include_directories(${target} PRIVATE ${source_roots})
    target_compile_definitions(${target} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${target} PUBLIC Threads::Threads)
    if(NOT WIN32)
        target_link_libraries(${target} PRIVATE ${CMAKE_DL_LIBS})
    endif()
    sushilog_apply_build_policy(${target})
    set_target_properties(${target} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        EXPORT_NAME ${export_name})
endfunction()

function(sushilog_compose_library)
    # Before 1.0 a minor release may break the ABI, so the minor number is part of the name.
    if(PROJECT_VERSION_MAJOR EQUAL 0)
        set(soversion "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    else()
        set(soversion "${PROJECT_VERSION_MAJOR}")
    endif()

    sushilog_add_library_target(sushilog SHARED SushiLog)
    target_compile_definitions(sushilog PRIVATE SUSHILOG_EXPORTS)
    set_target_properties(sushilog PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${soversion}
        OUTPUT_NAME sushilog)

    # SushiLog::text formats text without the logger, so a project builds messages alone.
    add_library(sushilog_text INTERFACE)
    add_library(SushiLog::text ALIAS sushilog_text)
    target_include_directories(sushilog_text INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/modules/core/text/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    set_target_properties(sushilog_text PROPERTIES EXPORT_NAME text)

    sushilog_add_library_target(sushilog_static STATIC SushiLogStatic)
    target_compile_definitions(sushilog_static PUBLIC SUSHILOG_STATIC)
    set_target_properties(sushilog_static PROPERTIES OUTPUT_NAME sushilog_static)
endfunction()
