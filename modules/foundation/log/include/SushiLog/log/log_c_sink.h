/**************************************************************************/
/* log_c_sink.h                                                           */
/**************************************************************************/
/*                          This file is part of:                         */
/*                                SushiLog                                */
/*                https://github.com/SushiSystems/SushiLog                */
/*                         https://sushisystems.io                        */
/**************************************************************************/
/* Copyright (c) 2026-present Mustafa Garip & Sushi Systems               */
/*                                                                        */
/* Licensed under the PolyForm Noncommercial License 1.0.0 (the           */
/* "License"); you may not use this file except in compliance with the    */
/* License. You may obtain a copy of the License at                       */
/*                                                                        */
/*     https://polyformproject.org/licenses/noncommercial/1.0.0           */
/*                                                                        */
/* Noncommercial use is free. Commercial use requires a separate licence  */
/* from Sushi Systems; see COMMERCIAL.md. The software is provided        */
/* "as is", without warranty of any kind.                                 */
/**************************************************************************/

/**
 * @file log_c_sink.h
 * @brief Declares the C interface through which a host outside C++ or a plug-in joins the logger.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <SushiLog/log/export.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Holds one record for a C sink; no string is NUL-terminated.
 *
 * A later version only adds fields at the end. A reader reads only the fields that both its
 * own sizeof(SushiLogRecord) and @c size cover.
 */
typedef struct SushiLogRecord
{
    uint32_t size;       /**< sizeof(SushiLogRecord) of the logger that filled the record. */
    uint8_t level;       /**< The level, numbered as SushiLog::LogLevel: Trace 0 to Fatal 5. */
    const char* project; /**< The project of the record's category. */
    size_t project_size; /**< The length of project. */
    const char* module;  /**< The module of the record's category. */
    size_t module_size;  /**< The length of module. */
    const char* file;    /**< The file of the call, without its folders. */
    size_t file_size;    /**< The length of file. */
    uint32_t line;       /**< The line of the call. */
    uint32_t thread;     /**< The sending logger's number for the calling thread. */
    int64_t time_ns;     /**< System clock time of the call, ns since the Unix epoch. */
    const char* message; /**< The formatted message alone. */
    size_t message_size; /**< The length of message. */
    const char* text;    /**< The whole line of the record. */
    size_t text_size;    /**< The length of text. */
} SushiLogRecord;

/**
 * @brief Receives one record and the pointer given with it; it must not throw or unwind into
 *        the logger.
 */
typedef void (*SushiLogSinkFunction)(const SushiLogRecord* record, void* user);

/**
 * @brief Replaces the sink of the logger with @p function, called with @p user.
 * @param function A null function restores the default, which writes to stderr.
 */
SUSHILOG_API void sl_set_log_sink(SushiLogSinkFunction function, void* user);

/**
 * @brief Chooses whether a record reaches the sink before its log call returns.
 * @param synchronous Nonzero waits for the queue, then delivers on the calling thread.
 */
SUSHILOG_API void sl_set_log_synchronous(int synchronous);

/**
 * @brief Enters another logger's record into this logger's queue, as log_forward_sink() does.
 * @param user Unused; present so the function is a SushiLogSinkFunction.
 */
SUSHILOG_API void sl_forward_record(const SushiLogRecord* record, void* user);

#ifdef __cplusplus
}
#endif
