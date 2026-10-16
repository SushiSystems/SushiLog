/**************************************************************************/
/* SushiLog.hpp                                                           */
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
 * @file SushiLog.hpp
 * @brief Includes every public C++ header of SushiLog; log_c_sink.h is included on its own.
 * @author Mustafa Garip
 * @see modules/foundation/umbrella/README.md
 */

#pragma once

#include <SushiLog/log/delegate.hpp>
#include <SushiLog/log/export.hpp>
#include <SushiLog/log/log_abi.hpp>
#include <SushiLog/log/log_calls.hpp>
#include <SushiLog/log/log_category.hpp>
#include <SushiLog/log/log_format.hpp>
#include <SushiLog/log/log_front.hpp>
#include <SushiLog/log/log_level.hpp>
#include <SushiLog/log/log_project.hpp>
#include <SushiLog/log/log_record.hpp>
#include <SushiLog/log/log_sink.hpp>
