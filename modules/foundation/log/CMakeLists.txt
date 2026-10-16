# CMakeLists.txt
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.

sushilog_add_module(
    NAME log TIER foundation
    SOURCES
        forward_sink.cpp
        level_environment.cpp
        level_name.cpp
        level_table.cpp
        library_path.cpp
        log_abi.cpp
        log_c_sink.cpp
        log_calls.cpp
        log_front.cpp
        log_queue.cpp
        log_writer.cpp
        logger.cpp
        own_record.cpp
        record_format.cpp
        sink_holder.cpp
        stderr_line.cpp)
