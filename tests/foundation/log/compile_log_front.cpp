/**************************************************************************/
/* compile_log_front.cpp                                                  */
/**************************************************************************/
/*                          This file is part of:                         */
/*                                SushiLog                                */
/*                https://github.com/SushiSystems/SushiLog                */
/*                         https://sushisystems.io                        */
/**************************************************************************/
/* Copyright (c) 2026-present Mustafa Garip & Sushi Systems               */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License"); you    */
/* may not use this file except in compliance with the License. You may   */
/* obtain a copy of the License at                                        */
/*                                                                        */
/*     https://www.apache.org/licenses/LICENSE-2.0                        */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/**************************************************************************/

/**
 * @file compile_log_front.cpp
 * @brief Checks at compile time the values of log_level_ceiling.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/log_front.hpp>

static_assert(SushiLog::log_level_ceiling(false) == SushiLog::LogLevel::Error);

#ifdef NDEBUG
static_assert(SushiLog::log_level_ceiling(true) == SushiLog::LogLevel::Debug);
#else
static_assert(SushiLog::log_level_ceiling(true) == SushiLog::LogLevel::Trace);
#endif
