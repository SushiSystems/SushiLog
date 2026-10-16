# SushiLogOptions.cmake
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

# Declares every option a person or the CLI sets. Included before project().

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(SL_IS_TOP_LEVEL ON)
else()
    set(SL_IS_TOP_LEVEL OFF)
endif()

option(SL_BUILD_TESTS "Build the test programs" ${SL_IS_TOP_LEVEL})
option(SL_INSTALL "Add the install rules and the package files" ON)

# address: AddressSanitizer with UndefinedBehaviorSanitizer; thread: ThreadSanitizer.
set(SL_SANITIZE "" CACHE STRING "Sanitizer build: empty, address or thread")
set_property(CACHE SL_SANITIZE PROPERTY STRINGS "" address thread)
