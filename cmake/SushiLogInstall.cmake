# SushiLogInstall.cmake
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

# Installs and exports both library targets, their headers and the package files.

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(SL_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/SushiLog"
    CACHE STRING "Where the SushiLog CMake package files are installed")

# Installs sushilog (its DLL under bin/ on Windows), sushilog_static, every public include
# root, the export set and the package configuration. Call it after sushilog_compose_library().
function(sushilog_install_package)
    get_property(include_roots GLOBAL PROPERTY SUSHILOG_INCLUDE_ROOTS)

    install(TARGETS sushilog sushilog_static sushilog_text
        EXPORT SushiLogTargets
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    foreach(include_root IN LISTS include_roots)
        install(DIRECTORY "${include_root}/"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
    endforeach()

    install(EXPORT SushiLogTargets
        FILE SushiLogTargets.cmake
        NAMESPACE SushiLog::
        DESTINATION "${SL_INSTALL_CMAKEDIR}")

    # The build tree is exported too, so a project that adds SushiLog as a subdirectory and
    # exports its own build tree finds SushiLog's targets in an export set.
    export(EXPORT SushiLogTargets
        FILE "${PROJECT_BINARY_DIR}/SushiLogTargets.cmake"
        NAMESPACE SushiLog::)

    configure_package_config_file(
        "${SL_CMAKE_DIR}/SushiLogConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/SushiLogConfig.cmake"
        INSTALL_DESTINATION "${SL_INSTALL_CMAKEDIR}")

    # Before 1.0 a minor release may break the interface, so 0.1 is not handed 0.2.
    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/SushiLogConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion)

    install(FILES
        "${PROJECT_BINARY_DIR}/SushiLogConfig.cmake"
        "${PROJECT_BINARY_DIR}/SushiLogConfigVersion.cmake"
        DESTINATION "${SL_INSTALL_CMAKEDIR}")
endfunction()
