# CMakeLists.txt
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

cmake_minimum_required(VERSION 3.21)

set(SL_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include("${SL_CMAKE_DIR}/SushiLogOptions.cmake")

project(SushiLog VERSION 0.2.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("${SL_CMAKE_DIR}/SushiLogBuildPolicy.cmake")
include("${SL_CMAKE_DIR}/SushiLogRuntimeFiles.cmake")
include("${SL_CMAKE_DIR}/SushiLogModule.cmake")
include("${SL_CMAKE_DIR}/SushiLogLibrary.cmake")
include("${SL_CMAKE_DIR}/SushiLogInstall.cmake")
include("${SL_CMAKE_DIR}/SushiLogTests.cmake")

add_subdirectory(modules)
sushilog_compose_library()

if(SL_INSTALL)
    sushilog_install_package()
endif()

if(SL_BUILD_TESTS)
    add_subdirectory(tests)
endif()
