/**************************************************************************/
/* forward_sink.hpp                                                       */
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
 * @file forward_sink.hpp
 * @brief Declares forwarded_record, which turns another logger's record into one of this logger.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#pragma once

#include <SushiLog/log/log_record.hpp>

#include "record_format.hpp"

namespace SushiLog::Detail
{
    /**
     * @brief Returns @p record as this logger queues it: its level, category, call site, time
     *        and message kept, the calling thread numbered here and the line written here.
     *
     * The sender's thread and text are ignored, and no level gate applies.
     */
    [[nodiscard]] QueuedRecord forwarded_record(const LogRecord& record);
} // namespace SushiLog::Detail
