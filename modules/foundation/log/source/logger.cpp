/**************************************************************************/
/* logger.cpp                                                             */
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
 * @file logger.cpp
 * @brief Defines logger(), the one logger of the process, and the end of its thread.
 * @author Mustafa Garip
 * @see modules/foundation/log/README.md
 */

#include "logger.hpp"

namespace SushiLog::Detail
{
    namespace
    {
        /** @brief Shuts the writer it was given down when the library's statics die. */
        class WriterLifetime final
        {
            public:
                /** @brief Holds @p writer, which it shuts down on destruction. */
                explicit WriterLifetime(LogWriter& writer) noexcept : writer_(writer)
                {

                }

                WriterLifetime(const WriterLifetime&) = delete;
                WriterLifetime& operator=(const WriterLifetime&) = delete;
                WriterLifetime(WriterLifetime&&) = delete;
                WriterLifetime& operator=(WriterLifetime&&) = delete;

                ~WriterLifetime()
                {
                    writer_.shut_down();
                }

            private:
                /// @brief The writer to shut down.
                LogWriter& writer_;
        };
    } // namespace

    Logger& logger()
    {
        // Never destroyed, so a log call is valid during static initialisation and exit.
        static Logger* const instance = new Logger;
        static const WriterLifetime lifetime(instance->writer);
        return *instance;
    }
} // namespace SushiLog::Detail
