/**************************************************************************/
/* value_text.hpp                                                         */
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
 * @file value_text.hpp
 * @brief Declares the helpers that write a value as text before it reaches a format.
 * @author Mustafa Garip
 * @see modules/core/text/README.md
 */

#pragma once

#include <cstdint>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

namespace SushiLog
{
    /** @brief Returns @p value with exactly @p precision digits after the point, as {:.Nf}. */
    inline std::string fixed(double value, int precision)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        return stream.str();
    }

    /** @brief Returns @p value in the shorter of fixed and scientific form, as {:.Ng}. */
    inline std::string general(double value, int precision = 6)
    {
        std::ostringstream stream;
        stream << std::setprecision(precision) << value;
        return stream.str();
    }

    /** @brief Returns @p value in scientific form with @p precision digits after the point, as {:.Ne}. */
    inline std::string scientific(double value, int precision)
    {
        std::ostringstream stream;
        stream << std::scientific << std::setprecision(precision) << value;
        return stream.str();
    }

    /** @brief Returns @p value in lower-case hexadecimal, zero-padded to @p width digits, as {:0Nx}. */
    inline std::string hex(std::uint64_t value, int width)
    {
        std::ostringstream stream;
        stream << std::hex << std::nouppercase << std::setw(width) << std::setfill('0') << value;
        return stream.str();
    }

    /**
     * @brief Returns @p value right-aligned in @p width characters, as {:>N}; longer text stays whole.
     * @tparam T A type an output stream can write.
     */
    template <typename T>
    std::string right(const T& value, int width)
    {
        std::ostringstream stream;
        stream << std::right << std::setw(width) << value;
        return stream.str();
    }

    /**
     * @brief Returns @p value left-aligned in @p width characters, as {:<N}; longer text stays whole.
     * @tparam T A type an output stream can write.
     */
    template <typename T>
    std::string left(const T& value, int width)
    {
        std::ostringstream stream;
        stream << std::left << std::setw(width) << value;
        return stream.str();
    }
} // namespace SushiLog
