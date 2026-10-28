# CMakeLists.txt
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

cmake_minimum_required(VERSION 3.21)

# An out-of-tree consumer of an INSTALLED SushiLog: find_package and nothing else. It builds one
# program against each target and registers both as tests. `sl test --package` installs the
# build tree, builds this project against the install and runs the two tests.

project(SushiLogPackageConsumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The installed library is built under the release C runtime and no iterator checking in every
# build type (cmake/SushiLogAbiPinning.cmake); the consumer takes the same.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release RelWithDebInfo MinSizeRel "")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    add_compile_definitions("$<$<COMPILE_LANGUAGE:CXX>:_ITERATOR_DEBUG_LEVEL=0>")
endif()

# The version is the one this tree builds, so the test follows each release without an edit.
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/../../CMakeLists.txt" project_line
    REGEX "^project[(]SushiLog VERSION [0-9.]+")
string(REGEX MATCH "[0-9]+[.][0-9]+[.][0-9]+" expected_version "${project_line}")
find_package(SushiLog ${expected_version} EXACT REQUIRED)

# A consumer inherits no warning, sanitizer or policy target of SushiLog's own tree.
foreach(target IN ITEMS SushiLog::SushiLog SushiLog::SushiLogStatic)
    get_target_property(options ${target} INTERFACE_COMPILE_OPTIONS)
    get_target_property(links ${target} INTERFACE_LINK_LIBRARIES)
    foreach(leak IN ITEMS -Werror -fsanitize /WX sushilog_warnings sushilog_sanitizers)
        if("${options};${links}" MATCHES "${leak}")
            message(FATAL_ERROR "${target} hands its consumer '${leak}': ${options};${links}")
        endif()
    endforeach()
endforeach()

enable_testing()

add_executable(sushilog_consumer_shared consumer.cpp)
target_link_libraries(sushilog_consumer_shared PRIVATE SushiLog::SushiLog)
target_compile_definitions(sushilog_consumer_shared PRIVATE CONSUMER_TARGET="shared")
if(WIN32)
    add_custom_command(TARGET sushilog_consumer_shared POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:sushilog_consumer_shared>
            $<TARGET_FILE_DIR:sushilog_consumer_shared>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endif()
add_test(NAME Small_Package.LogsThroughTheSharedTarget COMMAND sushilog_consumer_shared)
set_tests_properties(Small_Package.LogsThroughTheSharedTarget PROPERTIES
    LABELS small TIMEOUT 60)

add_executable(sushilog_consumer_static consumer.cpp)
target_link_libraries(sushilog_consumer_static PRIVATE SushiLog::SushiLogStatic)
target_compile_definitions(sushilog_consumer_static PRIVATE CONSUMER_TARGET="static")
add_test(NAME Small_Package.LogsThroughTheStaticTarget COMMAND sushilog_consumer_static)
set_tests_properties(Small_Package.LogsThroughTheStaticTarget PROPERTIES
    LABELS small TIMEOUT 60)
