# CMakeLists.txt
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.

sushilog_add_module_tests(
    MODULE log
    SOURCES
        compile_delegate.cpp
        compile_export.cpp
        compile_log_category.cpp
        compile_log_front.cpp
        compile_log_project.cpp
        compile_log_record.cpp
        compile_log_sink.cpp
        test_delegate.cpp
        test_level_setting.cpp
        test_log_abi.cpp
        test_log_c_sink.cpp
        test_log_capture.cpp
        test_log_clock.cpp
        test_log_calls/test_environment.cpp
        test_log_calls/test_exit.cpp
        test_log_calls/test_fatal.cpp
        test_log_calls/test_forward.cpp
        test_log_calls/test_levels.cpp
        test_log_calls/test_queue.cpp
        test_log_calls/test_record_line.cpp
        test_log_calls/test_reentry.cpp
        test_log_calls/test_sink.cpp
        test_log_executor.cpp
        test_log_front.cpp
        test_log_manual.cpp
        test_log_level.cpp
        test_thread_numbers.cpp)
