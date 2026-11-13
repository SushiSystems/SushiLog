# SushiLogTests.cmake
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.

# One function builds the test program of a module. The tests of a module sit in
# tests/<tier>/<module>/, the mirror of its folder under modules/, and build one program there.
#
# A test's size is its GoogleTest suite-name prefix: Small_*, Medium_* and Large_*, alone or after
# Characterization_ (a pin before a refactor, with its backlog entry), each with its own label and
# a timeout per case. The cases of a program run in shards per size,
# <program>.<size>_shard_<i>of<N>. A test named in SL_QUARANTINED_TESTS keeps its size and gains
# the quarantined label, which leaves it out of every gating run, and runs apart from the shards.
# `sl test --label <size>` runs one label.

# Suite.Case names of the intermittent tests, each with its known issue
# in docs/backlog/.
set(SL_QUARANTINED_TESTS "")

if(NOT SL_BUILD_TESTS)
    function(sushilog_add_module_tests)
    endfunction()
    return()
endif()

enable_testing()

# The pinned release is built with the tests, so it shares their configuration and runtime.
if(NOT TARGET GTest::gtest)
    include(FetchContent)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
        URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# The cases one CTest test runs, per size. A process pays its start-up once, so a case run alone is
# mostly start-up. A shard runs its share of the cases in one process: a small case is a few
# milliseconds, so 300 of them are a few seconds and the run still spreads over every core; a
# medium case uses threads or files and takes up to 10 s, so 25 keep a shard to a few minutes at
# most; a large case takes up to 120 s and runs alone.
set(SL_SHARD_CASES_SMALL 300)
set(SL_SHARD_CASES_MEDIUM 25)
set(SL_SHARD_CASES_LARGE 1)

# Where each shard writes its per-case GoogleTest XML; the time reader reads the folder, so the
# per-case limits of the testing skill still apply to a case run inside a shard.
set(SL_GTEST_RESULTS_DIR "${PROJECT_BINARY_DIR}/test_results/gtest")

# sushilog_scan_test_sources(<declared_var> <source>...)
#
# Sets <declared_var> to one entry per declaration, named by the suite or instantiation prefix. A
# TEST_P or TYPED_TEST_P counts once however many instances it gets, so the list is a floor of the
# case count.
function(sushilog_scan_test_sources declared_var)
    set(declared "")
    foreach(source IN LISTS ARGN)
        file(READ "${source}" text)
        string(REGEX MATCHALL "TYPED_TEST(_P)?[ \t]*[(][ \t\r\n]*[A-Za-z0-9_]+" typed "${text}")
        string(REGEX MATCHALL "(^|[^A-Za-z0-9_])TEST(_F|_P)?[ \t]*[(][ \t\r\n]*[A-Za-z0-9_]+" plain "${text}")
        foreach(use IN LISTS typed plain)
            string(REGEX REPLACE "^[^(]*[(][ \t\r\n]*" "" suite "${use}")
            list(APPEND declared "${suite}")
        endforeach()
        string(REGEX MATCHALL
            "INSTANTIATE_(TYPED_)?TEST_(SUITE|CASE)_P[ \t]*[(][ \t\r\n]*[A-Za-z0-9_]+[ \t\r\n]*,[ \t\r\n]*[A-Za-z0-9_]+"
            instances "${text}")
        foreach(use IN LISTS instances)
            string(REGEX REPLACE "^[^(]*[(][ \t\r\n]*" "" arguments "${use}")
            string(REGEX REPLACE "[ \t\r\n]*,.*$" "" prefix "${arguments}")
            if(prefix MATCHES "^(Characterization_)?(Small|Medium|Large)_")
                list(APPEND declared "${prefix}")
            endif()
        endforeach()
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source}")
    endforeach()
    set(${declared_var} "${declared}" PARENT_SCOPE)
endfunction()

# Registers one CTest test that runs <program> with <filter>, as shard <index> of <shards>.
# GoogleTest splits the filtered cases round-robin by GTEST_TOTAL_SHARDS and GTEST_SHARD_INDEX,
# and the test writes its per-case XML under SL_GTEST_RESULTS_DIR.
function(sushilog_add_shard_test program name filter labels timeout index shards)
    add_test(NAME ${name}
        COMMAND ${program} --gtest_filter=${filter}
            --gtest_output=xml:${SL_GTEST_RESULTS_DIR}/${name}.xml
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:${program}>")
    set_tests_properties(${name} PROPERTIES
        LABELS "${labels}"
        TIMEOUT ${timeout}
        ENVIRONMENT "GTEST_TOTAL_SHARDS=${shards};GTEST_SHARD_INDEX=${index}")
endfunction()

# Registers the cases of <program> of one size, in shards with the size's label and a timeout of
# the case timeout times the cases of a shard. A suite opens with the size prefix, alone or after
# Characterization_, so Characterization_Medium_X is a medium test; an instantiation prefix may
# carry it too. The quarantined cases of the size run apart in one test that also carries
# `quarantined`, and a GoogleTest filter after '-' leaves them out of the shards. <size_group> is
# "<prefix>|<label>|<case timeout>". This is the one function that registers shards.
function(sushilog_register_size_shards program module size_group declared)
    string(REPLACE "|" ";" parts "${size_group}")
    list(GET parts 0 prefix)
    list(GET parts 1 label)
    list(GET parts 2 case_timeout)
    string(TOUPPER "${label}" upper)

    set(cases 0)
    foreach(name IN LISTS declared)
        if(name MATCHES "^(Characterization_)?${prefix}")
            math(EXPR cases "${cases} + 1")
        endif()
    endforeach()
    if(cases LESS 1)
        return()
    endif()

    set(filter "${prefix}*:*/${prefix}*:Characterization_${prefix}*:*/Characterization_${prefix}*")
    set(labels "${label}" "${module}")

    # An instantiated suite is named <instance>/<suite>, so each name also carries the slash.
    set(quarantined "")
    set(quarantined_cases 0)
    foreach(name IN LISTS SL_QUARANTINED_TESTS)
        string(REGEX REPLACE "[.].*$" "" suite "${name}")
        if(name MATCHES "^(Characterization_)?${prefix}" AND suite IN_LIST declared)
            list(APPEND quarantined "${name}" "*/${name}")
            math(EXPR quarantined_cases "${quarantined_cases} + 1")
        endif()
    endforeach()
    if(quarantined)
        list(JOIN quarantined ":" quarantined_filter)
        string(APPEND filter "-${quarantined_filter}")
        math(EXPR quarantined_timeout "${case_timeout} * ${quarantined_cases}")
        sushilog_add_shard_test(${program} ${program}.${label}_quarantined
            "${quarantined_filter}" "${labels};quarantined" ${quarantined_timeout} 0 1)
        math(EXPR cases "${cases} - ${quarantined_cases}")
    endif()
    if(cases LESS 1)
        set(cases 1)
    endif()

    math(EXPR shards "(${cases} + ${SL_SHARD_CASES_${upper}} - 1) / ${SL_SHARD_CASES_${upper}}")
    math(EXPR shard_cases "(${cases} + ${shards} - 1) / ${shards}")
    math(EXPR timeout "${case_timeout} * ${shard_cases}")
    math(EXPR last "${shards} - 1")
    foreach(index RANGE ${last})
        sushilog_add_shard_test(${program} ${program}.${label}_shard_${index}of${shards}
            "${filter}" "${labels}" ${timeout} ${index} ${shards})
    endforeach()
endfunction()

# Registers the cases of <program> in shards per size: Small_* with 1 second per case, Medium_*
# with 10 and Large_* with 120. DECLARED lists the declarations the shards are sized from
# (sushilog_scan_test_sources); MODULE is the label of each test.
function(sushilog_register_sized_tests program module declared)
    set(groups "Small_|small|1" "Medium_|medium|10" "Large_|large|120")
    foreach(group IN LISTS groups)
        sushilog_register_size_shards(${program} "${module}" "${group}" "${declared}")
    endforeach()
endfunction()

# sushilog_add_module_tests(MODULE <module> SOURCES <file>...)
#
# Called from tests/<tier>/<module>/CMakeLists.txt, with paths relative to that folder. Builds
# sl_<module>_tests against SushiLog::SushiLog, copies the DLL beside it, refuses a suite with
# no size prefix, and registers every test with its size, in shards.
function(sushilog_add_module_tests)
    cmake_parse_arguments(ARG "" "MODULE" "SOURCES" ${ARGN})
    if("${ARG_MODULE}" STREQUAL "" OR NOT ARG_SOURCES)
        message(FATAL_ERROR "sushilog_add_module_tests(): MODULE and SOURCES are required.")
    endif()
    set(program "sl_${ARG_MODULE}_tests")
    set(sources "")
    foreach(source IN LISTS ARG_SOURCES)
        list(APPEND sources "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
    endforeach()

    add_executable(${program} ${sources})
    target_link_libraries(${program} PRIVATE SushiLog::SushiLog GTest::gtest_main)
    target_include_directories(${program} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    sushilog_apply_build_policy(${program})
    sushilog_copy_runtime_files(${program})

    # A suite with no size prefix would register no test, so the build refuses it.
    set(stamp "${CMAKE_CURRENT_BINARY_DIR}/${program}_sizes.stamp")
    list(JOIN sources "|" joined_sources)
    add_custom_command(
        OUTPUT "${stamp}"
        COMMAND ${CMAKE_COMMAND} "-DSOURCES=${joined_sources}" "-DSTAMP=${stamp}"
            -P "${SL_CMAKE_DIR}/CheckTestSizes.cmake"
        DEPENDS ${sources} "${SL_CMAKE_DIR}/CheckTestSizes.cmake"
        COMMENT "Checking the test sizes of ${program}"
        VERBATIM)
    add_custom_target(${program}_sizes DEPENDS "${stamp}")
    add_dependencies(${program} ${program}_sizes)

    sushilog_scan_test_sources(declared ${sources})
    sushilog_register_sized_tests(${program} ${ARG_MODULE} "${declared}")
endfunction()
