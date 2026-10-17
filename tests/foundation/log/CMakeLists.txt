# CMakeLists.txt
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

sushilog_add_module_tests(
    MODULE log
    SOURCES
        test_delegate.cpp
        test_export.cpp
        test_log_abi.cpp
        test_log_c_sink.cpp
        test_log_calls/test_environment.cpp
        test_log_calls/test_exit.cpp
        test_log_calls/test_fatal.cpp
        test_log_calls/test_forward.cpp
        test_log_calls/test_levels.cpp
        test_log_calls/test_queue.cpp
        test_log_calls/test_record_line.cpp
        test_log_calls/test_reentry.cpp
        test_log_calls/test_sink.cpp
        test_log_category.cpp
        test_log_front.cpp
        test_log_level.cpp
        test_log_project.cpp
        test_log_record.cpp
        test_log_sink.cpp)
